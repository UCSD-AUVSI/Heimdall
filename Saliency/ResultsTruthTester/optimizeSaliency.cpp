#include "optimizeSaliency.hpp"
#include "SharedUtils/optimization/MetropolisMonteCarlo.hpp"
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include <iostream>
using std::cout; using std::endl;
#include "SharedUtils/SharedUtils.hpp"
#include <thread>

const int DEFAULT_ASSUMED_NUM_FALSE_POSITIVES = 20;


double CalculateSaliencyFitnessScore(const SaliencyExperimentResults & results, int allowed_false_positives)
{
	double numerator,denominator;
	numerator = ((double)(results.numSuccesses)) + 0.7*((double)results.numFailures_PaddingFailures) + 0.4*((double)results.numCloseCallsForCroppingFailures) + 0.06*((double)results.numFailures_TooBig);
	denominator = (double)(MAX(results.numFailures_totalDetectionFailures, allowed_false_positives) + 1);
	
	return pow(numerator,5.3) / pow(denominator, 0.5);
}

void OptimizingSystemState_Saliency::GenerateNewArgs()
{
	assert(OptimizedArgs.size() == OptimizedArgsUpdateSteps.size());
	for(int ii=0; ii<OptimizedArgs.size(); ii++) {
		OptimizedArgs[ii] += myRNG.rand_float(-1.0*OptimizedArgsUpdateSteps[ii], OptimizedArgsUpdateSteps[ii]);
	}
	ConstrainArgs();
}

void OptimizingSystemState_Saliency::GetArgsFrom(const OptimizingSystemState_Saliency & other)
{
	std::copy(other.OptimizedArgs.begin(), other.OptimizedArgs.end(), OptimizedArgs.begin());
}


void StartOptimizationLoop(GenericSaliencyImplementation* themodule,
							double initial_guess_for_temperature_beta,
							OptimizingSystemState_Saliency* initial_state,
							OptimizingSystemState_Saliency* later_states,
							std::vector<std::vector<unsigned char>*> & compressed_images,
							std::vector<std::string> & image_original_filenames,
							int maxLoopsBeforeQuitting,
							std::string truth_filename,
							std::string folder_to_save_output_if_desired)
{
	assert(themodule != nullptr);
	EnableKeyPressToExit(false);
	cout<<"beginning optimization loop!"<<endl;
	
	const bool MoreConsoleOutput = false;
	bool firstLoop = true;
	bool thisIsTheLastLoop = false;
	double firstScoreSaved = 0.0;
	int numLoops = 0;
	
	OptimizingSystemState_Saliency*& previous = initial_state;
	OptimizingSystemState_Saliency*& latest = later_states;
	
	AdaptiveMetropolisBeta negtrials_adjuster(initial_guess_for_temperature_beta);
	double scoreDelta;
//--------------------------------------------------------------------------------------------
	
	SaliencyOutput output;
	
	
	while((--maxLoopsBeforeQuitting) >= 0) {
		output.clear();
		if(global__key_has_been_pressed || maxLoopsBeforeQuitting == 0) {
			cout<<endl<<"======================================== "<<numLoops<<" experiments done!"<<endl;
			cout      <<"                      calculating results with final parameters..."<<endl;
			maxLoopsBeforeQuitting = 0;
			thisIsTheLastLoop = true;
		} else {
			cout<<"=============================== starting experiment number "<<(++numLoops)<<endl;
			if(firstLoop == false) {
				previous->GetArgsFrom(*latest);
				latest->GenerateNewArgs();
			}
		}
		
		// Get PythonSaliency output
		for(int jj=0; jj<compressed_images.size(); jj++) {
			
			themodule->ReceivedUpdatedArgs(latest->OptimizedArgs);
			cout<<"about to process image "<<jj<<endl;
			themodule->ProcessSaliency(cv::imdecode(*compressed_images[jj], CV_LOAD_IMAGE_UNCHANGED),
									output.CropsForImage(image_original_filenames[jj]),
									output.LocationsForImage(image_original_filenames[jj]));
			
			if(MoreConsoleOutput) {
				if(output.CropsForImage(image_original_filenames[jj]).empty() == false) {
					for(int ii=0; ii<output.LocationsForImage(image_original_filenames[jj]).size(); ii++) {
						cout<<image_original_filenames[jj]<<" crop"<<ii<<" found at geo-location: "<<
							output.LocationsForImage(image_original_filenames[jj])[ii].first
							<<","<<
							output.LocationsForImage(image_original_filenames[jj])[ii].second
							<<endl;
					}
				}
				else {
					cout << "no crops found in this image!" << endl;
				}
			}
			output.CompressCropsSoFar();
		}
		//cout<<"DONE RUNNING SALIENCY"<<endl;
		//cout<<"FOUND RESULTS FOR "<<output.numFullsizeImages()<<" IMAGES"<<endl;
		
		
		// Check output
		previous->score = latest->score;
		previous->results = latest->results;
		latest->results = GetSaliencyExperimentResults(truth_filename, output);
		
		cout << //"-----------------------------------------------------" << endl <<
			"TruePositives: " << latest->results.TruePositives() << ", FalsePositives: " << latest->results.FalsePositives() <<
			", FalseNegatives: " << latest->results.FalseNegatives() << endl;// <<
			//"-----------------------------------------------------" << endl;
		if(thisIsTheLastLoop || firstLoop) {
			std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
			std::cout << "number of actual truth targets (from truth file): " << latest->results.numTruthTargets << std::endl;
			std::cout << "num crops: " << latest->results.numDetectedThings << std::endl;
			std::cout << "num successes: " << latest->results.numSuccesses << std::endl;
			std::cout << "total num failures: " << (latest->results.numFailures_totalDetectionFailures+latest->results.numFailures_PaddingFailures) << std::endl;
			std::cout << "numCloseCallsForCroppingFailures: " << latest->results.numCloseCallsForCroppingFailures << std::endl;
			std::cout << "num detection failures or false alarms: " << latest->results.numFailures_totalDetectionFailures << std::endl;
			std::cout << "num padding failures: " << latest->results.numFailures_PaddingFailures << std::endl;
			std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
		}
		
		previous->score = latest->score;
		latest->score = CalculateSaliencyFitnessScore(latest->results, DEFAULT_ASSUMED_NUM_FALSE_POSITIVES);
		if(MoreConsoleOutput) {
			cout << "THIS SCORE: "<<latest->score<<endl;
		}
		
		if(firstLoop) {
			firstLoop = false;
			firstScoreSaved = latest->score;
		} else if(thisIsTheLastLoop == false) {
			scoreDelta = (latest->score - previous->score);
			
			bool accepted = ((scoreDelta >= 0.0) || latest->myRNG.rand_double(0.0,1.0) < exp(negtrials_adjuster.getPositiveBeta()*scoreDelta));
			
			negtrials_adjuster.UpdateTrials(scoreDelta, accepted, MoreConsoleOutput);
			
			if(accepted == false) {
				latest->GetArgsFrom(*previous);
			}
		}
		if(thisIsTheLastLoop) {
			scoreDelta = (latest->score - firstScoreSaved);
			cout<<"ORIGINAL PARAMETERS' SCORE: "<<firstScoreSaved<<endl;
			cout<<"FINAL PARAMETERS' SCORE:    "<<latest->score<<endl;
			cout<<"DELTA SCORE (FINAL-ORIGINAL) == "<<scoreDelta<<endl;
			
			if(check_if_directory_exists(folder_to_save_output_if_desired)) {
				for(int ii=0; ii<output.numFullsizeImages(); ii++) {
					for(int jj=0; jj<output.CompressedCropsForIdx(ii).size(); jj++) {
						std::string imgname(folder_to_save_output_if_desired+std::string("/")+output.ImgNameForIdx(ii)+std::string("_crop")+to_istring(jj)+std::string(".png"));
						cout<<"saving image: \""<<imgname<<"\""<<endl;
						cv::imwrite(imgname, cv::imdecode(*output.CompressedCropsForIdx(ii)[jj],CV_LOAD_IMAGE_UNCHANGED));
					}
				}
			}
		}
		
		cout<< (thisIsTheLastLoop ? "FINAL PARAMETERS: (" : "PARAMS SO FAR: (");
		for(int ii=0; ii<latest->OptimizedArgs.size(); ii++) {
			if(ii != 0){cout<<", ";}
			cout<<latest->OptimizedArgs[ii];
		}
		cout<<")"<<endl;
		
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
