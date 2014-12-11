#include <iostream>
using std::cout;
using std::endl;
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "Saliency/PythonSaliency/CPlusPlus_Heimdall_Interface/PythonSaliency.hpp"
#include "SharedUtils/GlobalVars.hpp"
#include "Saliency/ResultsTruthTester/SaliencyExperimentResults.hpp"
#include <thread>
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include <deque>


static int choose_a_random_training_subset_of_size = 8;


static std::string GetPathToTestExecutable(const char* argv0) {
	std::string fullpath = GetPathOfExecutable(argv0);
	std::string trimThis("/bin/test/testGeneticOptimizeCanny3D");
	if(fullpath.substr(fullpath.size()-trimThis.size()) != trimThis) {
		std::cout<<"WARNING: TEST EXECUTABLE NOT FOUND ???? error finding path to executable"<<std::endl;
	}
	fullpath.erase(fullpath.size()-trimThis.size());
	return fullpath;
}


class AdaptiveMetropolisBeta
{
	int totalDeltaTrials;
	int totalDeltaTrialsNegative;
	int totalDeltaTrialsNegativeAccepted;
	double METROPOLIS_BETA_CONST_MUST_BE_POSITIVE;
	std::deque<bool> lastFewNegativeTrialResults;
	double latestNegativeAcceptancePct;
	
public:
	int numNegTrialsBeforeAdjustingBeta;
	int maxStepsToConsiderWhenAdjustingBeta;
	double desiredNegativeAcceptancePercent;
	
	
	AdaptiveMetropolisBeta() :
		numNegTrialsBeforeAdjustingBeta(9),
		maxStepsToConsiderWhenAdjustingBeta(12),
		desiredNegativeAcceptancePercent(18.0),
		totalDeltaTrials(0),
		totalDeltaTrialsNegative(0),
		totalDeltaTrialsNegativeAccepted(0),
		METROPOLIS_BETA_CONST_MUST_BE_POSITIVE(50.0) {}
	
	
	double getPositiveBeta() {return METROPOLIS_BETA_CONST_MUST_BE_POSITIVE;}
	
	double getPositivePercent() {
		return 100.0 * ((double)(totalDeltaTrials - totalDeltaTrialsNegative)) / ((double)totalDeltaTrials);
	}
	double getNegativeAcceptancePercent() {
		return 100.0 * ((double)totalDeltaTrialsNegativeAccepted) / ((double)totalDeltaTrialsNegative);
	}
	
	void UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console)
	{
		totalDeltaTrials++;
		if(new_score_delta < 0.0) {
			totalDeltaTrialsNegative++;
			if(trial_was_accepted) {
				totalDeltaTrialsNegativeAccepted++;
			}
			if(lastFewNegativeTrialResults.size() == maxStepsToConsiderWhenAdjustingBeta) {
				lastFewNegativeTrialResults.pop_front();
			}
			lastFewNegativeTrialResults.push_back(trial_was_accepted);
			
			//update beta?
			if(totalDeltaTrialsNegative >= numNegTrialsBeforeAdjustingBeta) {
				latestNegativeAcceptancePct = 0.0;
				for(int ii=0; ii<lastFewNegativeTrialResults.size(); ii++)
					if(lastFewNegativeTrialResults[ii])
						latestNegativeAcceptancePct += 100.0;
				latestNegativeAcceptancePct /= (double)(lastFewNegativeTrialResults.size());
				double bdelta = 0.8*0.01*(latestNegativeAcceptancePct - desiredNegativeAcceptancePercent);
				METROPOLIS_BETA_CONST_MUST_BE_POSITIVE *= pow(1.0+bdelta, 1.0);
				if(METROPOLIS_BETA_CONST_MUST_BE_POSITIVE < 0.0000000001) {
					METROPOLIS_BETA_CONST_MUST_BE_POSITIVE = 0.0000000001;
				}
			}
		}
		cout<<"neg trials: "<<totalDeltaTrialsNegative<<",  +delta pct: "<<RoundDoubleToInteger(getPositivePercent())<<",  recent -delta accept pct: "<<RoundDoubleToInteger(latestNegativeAcceptancePct);
		if(new_score_delta < 0.0) {
			cout<<",  metropolis beta: "<<METROPOLIS_BETA_CONST_MUST_BE_POSITIVE;
		}
		cout<<endl;
		
		if(print_more_to_console) {
			if(new_score_delta < 0.0) {
				cout << "NEGATIVE STEP: SCORE DELTA: "<<new_score_delta<<(trial_was_accepted?" ACCEPTED~~~~~~~~~~~~~~********":" REJECTED~~~~~~~~~~~~~~")<<endl;
			} else {
				cout << "POSITIVE STEP: SCORE DELTA: "<<new_score_delta<<(trial_was_accepted?" ACCEPTED":" REJECTED")<<endl;
			}
		}
	}
};


class OptimizingSystemState_C3D
{
public:
	SaliencyExperimentResults results;
	std::vector<double> OptimizedArgs;
	double score;
	RNG_rand_r myRNG;
	
	OptimizingSystemState_C3D() :
		results(SaliencyExperimentResults()),
		myRNG(RNG_rand_r()),
		score(0.0) {
			OptimizedArgs.resize(4);
			OptimizedArgs[0] = 100.0; //cThreshLow
			OptimizedArgs[1] = 30.0; //cHighRatioTimes5
			OptimizedArgs[2] = 11.0; //blurRadius
			OptimizedArgs[3] = 20.0; //resizePct
		}
	
	void GenerateNewArgs(double scl = 1.0) {
		OptimizedArgs[0] += myRNG.rand_float(-6.7*scl, 6.7*scl);
		OptimizedArgs[1] += myRNG.rand_float(-2.0*scl, 2.0*scl);
		OptimizedArgs[2] += myRNG.rand_float(-1.1*scl, 1.1*scl); //proportionally, 0.733
		OptimizedArgs[3] += myRNG.rand_float(-0.9*scl, 0.9*scl); //proportionally, 1.333
		
		OptimizedArgs[0] = CLAMP(OptimizedArgs[0], 10.0, 1000.0);
		OptimizedArgs[1] = CLAMP(OptimizedArgs[1], 1.001, 200.0);
		OptimizedArgs[2] = CLAMP(OptimizedArgs[2], 3.0, 16.0);
		OptimizedArgs[3] = CLAMP(OptimizedArgs[3], 9.0, 37.0);
	}
	
	void GetArgsFrom(const OptimizingSystemState_C3D & other) {
		OptimizedArgs[0] = other.OptimizedArgs[0];
		OptimizedArgs[1] = other.OptimizedArgs[1];
		OptimizedArgs[2] = other.OptimizedArgs[2];
		OptimizedArgs[3] = other.OptimizedArgs[3];
	}
};


int main(int argc, char** argv)
{
	if(argc < 1) { std::cout<<"?? no arguments for executable??"<<std::endl; return 1; }
	path_to_HeimdallBuild_directory = new std::string(GetPathToTestExecutable(argv[0]));
	
	cout << "Optimize-Canny3D-Saliency" << endl;
	if(argc < 2) {
		consoleOutput.Level0() << "usage:  [PATH TO FOLDER WITH IMAGES]" << endl;
		return 1;
	}
	if(!check_if_directory_exists(argv[1])) {
		consoleOutput.Level0() << "ERROR: path \""<<argv[1]<<"\" not found!!" << endl;
		return 1;
	}
	EnableKeyPressToExit(false);
	
	std::vector<std::string> tImgPaths;
	std::vector<std::string> tImgFnames;
	std::vector<std::vector<unsigned char>*> pngImgs;
	
	std::vector<unsigned char> *newarr_writehere;
	std::vector<int> pngparam = std::vector<int>(2);
	pngparam[0] = CV_IMWRITE_PNG_COMPRESSION; pngparam[1] = 3; //0-9... 9 is smallest compressed size
	int compressedSizeOfAllImages = 0;
	
	tinydir_dir dir;
	tinydir_open(&dir, argv[1]);
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.' && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			tImgFnames.push_back(file.name);
			tImgPaths.push_back(file.path);
			
			if(choose_a_random_training_subset_of_size <= 0) {
				newarr_writehere = new std::vector<unsigned char>();
				cv::imencode(".png", cv::imread(file.path,CV_LOAD_IMAGE_COLOR), *newarr_writehere, pngparam);
				pngImgs.push_back(newarr_writehere);
				compressedSizeOfAllImages += (newarr_writehere->size());
				cout<<"found "<<pngImgs.size()<<" images so far, total size: "<<static_cast<int>(((double)compressedSizeOfAllImages)*0.001*0.001)<<" megabytes"<<endl;
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	
	if(choose_a_random_training_subset_of_size > 0) {
		std::vector<std::string> tImgFnames_Copy(tImgFnames);
		tImgFnames.clear();
		while(pngImgs.size() != choose_a_random_training_subset_of_size)
		{
			RNG_rand_r rngg;
			int newidx = rngg.rand_int(0, ((int)tImgPaths.size()) - 1);
			
			newarr_writehere = new std::vector<unsigned char>();
			cv::imencode(".png", cv::imread(tImgPaths[newidx],CV_LOAD_IMAGE_COLOR), *newarr_writehere, pngparam);
			compressedSizeOfAllImages += (newarr_writehere->size());
			
			pngImgs.push_back(newarr_writehere);
			tImgFnames.push_back(tImgFnames_Copy[newidx]);
			
			tImgPaths.erase(      tImgPaths.begin() + newidx);
			tImgFnames_Copy.erase(tImgFnames_Copy.begin() + newidx);
			
			cout<<"chose image \""<<tImgFnames.back()<<"\" for study, total size so far: "<<static_cast<int>(((double)compressedSizeOfAllImages)*0.001*0.001)<<" megabytes"<<endl;
		}
	}
	assert(pngImgs.size() == tImgFnames.size());
	
	cout << "found " << pngImgs.size() << " images to test with!" << endl;
	exit(0);
	
//--------------------------------------------------------------------------------------------
	const bool MoreConsoleOutput = false;
	const std::string truthFilename("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	bool firstLoop = true;
	int maxLoopsBeforeQuitting = 1500000;
	bool thisIsTheLastLoop = false;
	double firstScoreSaved = 0.0;
	int numLoops = 0;
	
	OptimizingSystemState_C3D previous;
	OptimizingSystemState_C3D latest;
	AdaptiveMetropolisBeta negtrials_adjuster;
	double scoreDelta;
//--------------------------------------------------------------------------------------------
	
	SaliencyOutput output;
	
	
	while((--maxLoopsBeforeQuitting) >= 0) {
		output.clear();
		if(global__key_has_been_pressed) {
			cout<<endl<<"======================================== "<<numLoops<<" experiments done!"<<endl;
			cout      <<"                      calculating results with final parameters..."<<endl;
			maxLoopsBeforeQuitting = 0;
			thisIsTheLastLoop = true;
		} else {
			cout<<"=============================== starting experiment number "<<(++numLoops)<<endl;
			if(firstLoop == false) {
				previous.GetArgsFrom(latest);
				latest.GenerateNewArgs();
			}
		}
		
		// Get PythonSaliency output
		for(int jj=0; jj<pngImgs.size(); jj++) {
			pythonSaliency("Canny3D", "main.py", "doSaliency",
							cv::imdecode(*pngImgs[jj], CV_LOAD_IMAGE_COLOR),
							output.CropsForImage(tImgFnames[jj]),
							output.LocationsForImage(tImgFnames[jj]),
							&latest.OptimizedArgs);
			output.CompressCropsSoFar();
			if(MoreConsoleOutput) {
				if(output.CropsForImage(tImgFnames[jj]).empty() == false) {
					for(int ii=0; ii<output.CropsForImage(tImgFnames[jj]).size(); ii++) {
						cout<<tImgFnames[jj]<<" crop"<<to_istring(ii)<<" found at geo-location: "<<
							output.LocationsForImage(tImgFnames[jj])[ii].first
							<<","<<
							output.LocationsForImage(tImgFnames[jj])[ii].second
							<<endl;
					}
				}
				else {
					cout << "no crops found in this image!" << endl;
				}
			}
		}
		//cout<<"DONE RUNNING SALIENCY"<<endl;
		//cout<<"FOUND RESULTS FOR "<<output.numFullsizeImages()<<" IMAGES"<<endl;
		
		
		// Check output
		previous.score = latest.score;
		previous.results = latest.results;
		latest.results = GetSaliencyExperimentResults(truthFilename, output);
		
		cout << //"-----------------------------------------------------" << endl <<
			"TruePositives: " << latest.results.TruePositives() << ", FalsePositives: " << latest.results.FalsePositives() <<
			", FalseNegatives: " << latest.results.FalseNegatives() << endl;// <<
			//"-----------------------------------------------------" << endl;
		if(thisIsTheLastLoop || firstLoop) {
			std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
			std::cout << "number of actual truth targets (from truth file): " << latest.results.numTruthTargets << std::endl;
			std::cout << "num crops: " << latest.results.numDetectedThings << std::endl;
			std::cout << "num successes: " << latest.results.numSuccesses << std::endl;
			std::cout << "total num failures: " << (latest.results.numFailures_totalDetectionFailures+latest.results.numFailures_PaddingFailures) << std::endl;
			std::cout << "numCloseCallsForCroppingFailures: " << latest.results.numCloseCallsForCroppingFailures << std::endl;
			std::cout << "num detection failures or false alarms: " << latest.results.numFailures_totalDetectionFailures << std::endl;
			std::cout << "num padding failures: " << latest.results.numFailures_PaddingFailures << std::endl;
			std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
		}
		
		previous.score = latest.score;
		double numerator,denominator;
		numerator = ((double)(latest.results.numSuccesses)) + 0.7*((double)latest.results.numFailures_PaddingFailures) + 0.4*((double)latest.results.numCloseCallsForCroppingFailures);
		denominator = (double)(latest.results.numFailures_totalDetectionFailures + 1);
		latest.score = numerator/denominator;
		if(MoreConsoleOutput) {
			cout << "THIS SCORE: "<<latest.score<<",    numerator: "<<numerator<<", denom: "<<denominator<<endl;
		}
		
		if(firstLoop) {
			firstLoop = false;
			firstScoreSaved = latest.score;
		} else if(thisIsTheLastLoop == false) {
			scoreDelta = (latest.score - previous.score);
			
			bool accepted = ((scoreDelta >= 0.0) || latest.myRNG.rand_double(0.0,1.0) < exp(negtrials_adjuster.getPositiveBeta()*scoreDelta));
			
			negtrials_adjuster.UpdateTrials(scoreDelta, accepted, MoreConsoleOutput);
			
			if(accepted == false) {
				latest.GetArgsFrom(previous);
			}
		}
		if(thisIsTheLastLoop) {
			scoreDelta = (latest.score - firstScoreSaved);
			cout<<"ORIGINAL PARAMETERS' SCORE: "<<firstScoreSaved<<endl;
			cout<<"FINAL PARAMETERS' SCORE:    "<<latest.score<<endl;
			cout<<"DELTA SCORE (FINAL-ORIGINAL) == "<<scoreDelta<<endl;
		}
		
		cout<< (thisIsTheLastLoop ? "FINAL PARAMETERS:  (" : "PARAMS SO FAR:  (")
							     <<latest.OptimizedArgs[0]<<","<<latest.OptimizedArgs[1]
							<<","<<latest.OptimizedArgs[2]<<","<<latest.OptimizedArgs[3]<<")"<<endl;
		
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	return 0;
}

