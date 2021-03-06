#include "OptimizeableSaliency.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include <opencv/highgui.h>
#include <iostream>
#include <thread>
using std::cout; using std::endl;


const int NUM_SIMULTANEOUS_THREADS = 4;
#define DO_MULTITHREADING_SALIENCY_OPTIMIZATION 1


/*extern*/ std::string * OptimizeableSaliency_TruthFilename = nullptr;
/*extern*/ std::string * OptimizeableSaliency_FolderToSaveOutput = nullptr;

const int DEFAULT_ALLOWED_NUM_FALSE_POSITIVES = 30;
const int UPPER_LIMIT_OF_AVG_NUM_CROPS_PER_IMAGE = 6;


void OptimizeableSaliency_Params::CopyFromOther(Optimizer_Params const*const other)
{
	OptimizeableSaliency_Params const*const otherCasted = dynamic_cast<OptimizeableSaliency_Params const*const>(other);
	assert(otherCasted != nullptr);
	assert(otherCasted->params.empty() == false);
	params.resize(otherCasted->params.size());
	for(int ii=0; ii<otherCasted->params.size(); ii++) {
		params[ii] = otherCasted->params[ii];
	}
}

void OptimizeableSaliency_Params::Print(std::ostream & printHere)
{
	printHere<<"(";
	for(int ii=0; ii<params.size(); ii++) {
		if(ii != 0){printHere<<", ";}
		printHere<<params[ii];
	}
	printHere<<")"<<endl;
}

void OptimizeableSaliency_Params::GenerateNewArgs(double arg, bool fullRange)
{
	assert(params.size() == paramsStepSizes.size());
	
	if(fullRange) {
		std::vector<double> const* argsMin = nullptr;
		std::vector<double> const* argsMax = nullptr;
		GetArgConstraints(argsMin, argsMax);
		assert(argsMin != nullptr && argsMax != nullptr);
		
		if(arg > 0.0) {
			//this case used by S.A. post-warmup
			for(int ii=0; ii<params.size(); ii++) {
				double arghalfdiff = 0.5*((*argsMax)[ii] - (*argsMin)[ii]);
				params[ii] += RNG::rand_double_static(-1.0*arg*arghalfdiff, arg*arghalfdiff);
			}
			ConstrainArgs();
		} else {
			//this case used by S.A. warmup
			for(int ii=0; ii<params.size(); ii++) {
				params[ii] = RNG::rand_double_static((*argsMin)[ii], (*argsMax)[ii]);
			}
		}
		
	} else {
		if(arg > 0.0) {
			//this case isn't used
			for(int ii=0; ii<params.size(); ii++) {
				params[ii] += RNG::rand_double_static(-1.0*arg*paramsStepSizes[ii], arg*paramsStepSizes[ii]);
			}
		} else {
			//this case is used by MCMC fixed size
			for(int ii=0; ii<params.size(); ii++) {
				params[ii] += RNG::rand_double_static(-1.0*paramsStepSizes[ii], paramsStepSizes[ii]);
			}
		}
		ConstrainArgs();
	}
}

//===============================================================================================

void OptimizeableSaliency_Multithreaded::ProcessData(Optimizer_SourceData * givenData, Optimizer_Optimizee_Output * returnedOutput)
{
	assert(actualSaliencyModule != nullptr);
	OptimizeableSaliency_SourceData* givenDataCast = static_cast<OptimizeableSaliency_SourceData*>(givenData);
	OptimizeableSaliency_Output* returnedOutputCast = dynamic_cast<OptimizeableSaliency_Output*>(returnedOutput);
	assert(givenDataCast != nullptr && returnedOutputCast != nullptr);
	
//============================================================================================
    const int numGivenImages = ((int)givenDataCast->tImgFnames.size());
	bool these_params_experiment_aborted;
	int imgLoopImgNum;
	int imgLoopLim;
	int threadnum;
	std::vector<std::thread*> all_threads;
	std::vector<cv::Mat*> decoded_images;
	
	
	these_params_experiment_aborted = false;
	imgLoopLim = numGivenImages;
	if(imgLoopLim % NUM_SIMULTANEOUS_THREADS == 0) {
		imgLoopLim /= NUM_SIMULTANEOUS_THREADS;
	} else {
		imgLoopLim += (NUM_SIMULTANEOUS_THREADS - (imgLoopLim % NUM_SIMULTANEOUS_THREADS));
		imgLoopLim /= NUM_SIMULTANEOUS_THREADS;
	}
	for(int jj=0; jj<imgLoopLim; jj++) {
	
#if DO_MULTITHREADING_SALIENCY_OPTIMIZATION
#else
		std::vector< std::vector<cv::Mat>* > allReturnedCrops;
		std::vector< std::vector<std::pair<double,double>>* > allReturnedGeolocations;
#endif
		for(threadnum = 0; threadnum<NUM_SIMULTANEOUS_THREADS; threadnum++) {
			imgLoopImgNum = ((jj*NUM_SIMULTANEOUS_THREADS) + threadnum);
			if(imgLoopImgNum < numGivenImages) {
				consoleOutput.Level(3)<<"about to process image "<<imgLoopImgNum<<endl;
				decoded_images.push_back(givenDataCast->GetDecodedImage(imgLoopImgNum));
#if DO_MULTITHREADING_SALIENCY_OPTIMIZATION
				std::thread* newthr = 
					new std::thread(
									//lambda function to call the pure virtual function
									[&](cv::Mat * a1, std::vector<cv::Mat> * a2, std::vector<std::pair<double,double>> * a3, int a4){actualSaliencyModule->ProcessSaliency(a1,a2,a3,a4);},
									//the 3 arguments for ProcessSaliency
									decoded_images.back(),
									&(returnedOutputCast->CropsForImage(givenDataCast->tImgFnames[imgLoopImgNum])),
									&(returnedOutputCast->LocationsForImage(givenDataCast->tImgFnames[imgLoopImgNum])),
									imgLoopImgNum
									);
				all_threads.push_back(newthr);
#else
				allReturnedCrops.push_back(new std::vector<cv::Mat>());
				allReturnedGeolocations.push_back(new std::vector<std::pair<double,double>>());
				themodule->ProcessSaliency(
											decoded_images.back(),
											allReturnedCrops.back(),
											allReturnedGeolocations.back(),
											0);
#endif
			}
		}
		for(threadnum = 0; threadnum < all_threads.size(); threadnum++) {
			all_threads[threadnum]->join();
			delete all_threads[threadnum];
			all_threads[threadnum] = nullptr;
			if(givenDataCast->ImagesAreCompressed()) {
                delete decoded_images[threadnum];
                decoded_images[threadnum] = nullptr;
            }
		}
		all_threads.clear();
		decoded_images.clear();
        
		//returnedOutputCast->CompressCropsSoFar();
		
		int thisNumCrops = returnedOutputCast->NumCrops();
		if(thisNumCrops > (UPPER_LIMIT_OF_AVG_NUM_CROPS_PER_IMAGE*numGivenImages)) {
			cout<<"~~~~~~~~~~~~~ ABORTING THESE PARAMS: NUM CROPS IS ALREADY == "<<thisNumCrops<<endl;
			jj = imgLoopLim;
			these_params_experiment_aborted = true;
		}
	}
//============================================================================================
}

Optimizer_Params * OptimizeableSaliency_Multithreaded::CreateNewParams()
{
	assert(paramsInstanceForCreatingCopies != nullptr);
	return paramsInstanceForCreatingCopies->CreateInstance();
}

void OptimizeableSaliency_Multithreaded::ReceivedUpdatedArgs(Optimizer_Params const*const newArgs)
{
	assert(actualSaliencyModule != nullptr && newArgs != nullptr);
	const OptimizeableSaliency_Params* newArgsCast = dynamic_cast<const OptimizeableSaliency_Params*>(newArgs);
	actualSaliencyModule->ReceivedUpdatedArgs(newArgsCast->params);
}

//===============================================================================================

void OptimizeableSaliency_ResultsStats::CopyFromOther(Optimizer_ResultsStats const*const other)
{
	OptimizeableSaliency_ResultsStats const*const otherCasted = dynamic_cast<OptimizeableSaliency_ResultsStats const*const>(other);
	(*this) = (*otherCasted);
}


double OptimizeableSaliency_ResultsStats::CalculateFitnessScore()
{
	double perfect_successes_weighted =	 	((double)(DesiredTarget_Successes                 +DesiredFalseP_Successes))
									+ 0.7 *	((double)(DesiredTarget_CloseFailures_MinorPadding+DesiredFalseP_CloseFailures_MinorPadding))
									+ 0.35*	((double)(DesiredTarget_CloseFailures_PartialCrop +DesiredFalseP_CloseFailures_PartialCrop))
									+ 0.08*	((double)(DesiredTarget_CloseFailures_FarTooBig   +DesiredFalseP_CloseFailures_FarTooBig));
	
	//fix divide-by-zero errors
	perfect_successes_weighted = MAX(1e-6, perfect_successes_weighted);
	
	double perfect_precis = perfect_successes_weighted / ((double)( NumReturnedCrops ));
	perfect_precis *= perfect_precis; //make moderate precision scores more acceptable; at high precision the differences mean less
	
	double perfect_recall = perfect_successes_weighted / ((double)(Truth_DesiredTargets + Truth_DesiredFalsePs));
	
	double recall_weight    = 5.0; //recall is more important than precision
	double precision_weight = 1.0;
	
	/*
		Return weighted harmonic mean of precision and recall
	*/
	return (recall_weight+precision_weight) / ((recall_weight/perfect_recall) + (precision_weight/perfect_precis));
}


void OptimizeableSaliency_ResultsStats::Print(std::ostream & printHere, bool more_detailed)
{
	if(more_detailed) {
		printHere << "-----------------------------------------------------------------------------------------------------------------------" << endl;
		printHere << "number of actual truth targets (from truth file): " << Truth_DesiredTargets << endl;
		printHere << "number of actual truth desired-false-positives (from truth file): " << Truth_DesiredFalsePs << endl;
		printHere << "num crops: " << Total_Detections << endl;
		printHere << "num successes: targets: " << DesiredTarget_Successes << endl;
		printHere << "num successes: falseps: " << DesiredFalseP_Successes << endl;
		printHere << "total num nothing-useful-crops (unwanted-false-positives): " << Total_NothingUsefulFailures << endl;
		printHere << "partial-failures: targets: PartialCrops: " << DesiredTarget_CloseFailures_PartialCrop << endl;
		printHere << "partial-failures: targets: FarTooBigs: " << DesiredTarget_CloseFailures_FarTooBig << endl;
		printHere << "partial-failures: targets: MinorPaddings: " << DesiredTarget_CloseFailures_MinorPadding << endl;
		printHere << "partial-failures: falsepos: PartialCrops: " << DesiredFalseP_CloseFailures_PartialCrop << endl;
		printHere << "partial-failures: falsepos: FarTooBigs: " << DesiredFalseP_CloseFailures_FarTooBig << endl;
		printHere << "partial-failures: falsepos: MinorPaddings: " << DesiredFalseP_CloseFailures_MinorPadding << endl;
		printHere << "-----------------------------------------------------------" << endl;
		for(int ii=0; ii<DesiredTargets_Missed_IncludingCloseFailures.size(); ii++) {
			printHere<<"missed target in image \""<<DesiredTargets_Missed_IncludingCloseFailures[ii]<<"\""<<endl;
		}
		for(int ii=0; ii<DesiredFalsePs_Missed_IncludingCloseFailures.size(); ii++) {
			printHere<<"missed falsep in image \""<<DesiredFalsePs_Missed_IncludingCloseFailures[ii]<<"\""<<endl;
		}
		printHere << "-----------------------------------------------------------------------------------------------------------------------" << endl;
	} else {
		printHere << //"-----------------------------------------------------" << endl <<
			"TruePositives: " << TruePositives() << ", CloseErrors: " << NumCloseErrors() << ", FalsePositives: " << FalsePositives() <<
			", FalseNegatives: " << FalseNegatives() << endl;// <<
			//"-----------------------------------------------------" << endl;
	}
}

//===============================================================================================

//CalculateResults() is in a separate file: see "CalculateSaliencyExperimentResults"
//OptimizeableSaliency_Output::CalculateResults()


void OptimizeableSaliency_Output::SaveToDisk()
{
	if(OptimizeableSaliency_FolderToSaveOutput != nullptr) {
		if(check_if_directory_exists(*OptimizeableSaliency_FolderToSaveOutput)) {
			for(int ii=0; ii<numFullsizeImages(); ii++) {
				for(int jj=0; jj<CompressedCropsForIdx(ii).size(); jj++) {
					std::string imgname((*OptimizeableSaliency_FolderToSaveOutput)+std::string("/")+ImgNameForIdx(ii)+std::string("_crop")+to_istring(jj)+std::string(".png"));
					cout<<"saving image: \""<<imgname<<"\""<<endl;
					cv::imwrite(imgname, cv::imdecode(*CompressedCropsForIdx(ii)[jj],CV_LOAD_IMAGE_UNCHANGED));
				}
			}
		}
	}
}

void OptimizeableSaliency_Output::clear()
{
	auto cropsiter = resultCrops.begin();
	while(cropsiter != resultCrops.end()) {
		if(cropsiter->second != nullptr) {
			cropsiter->second->clear();
			delete cropsiter->second;
		}
		cropsiter = resultCrops.erase(cropsiter);
	}
	auto locationsiter = resultLocations.begin();
	while(locationsiter != resultLocations.end()) {
		if(locationsiter->second != nullptr) {
			locationsiter->second->clear();
			delete locationsiter->second;
		}
		locationsiter = resultLocations.erase(locationsiter);
	}
	clearCompressedCrops();
}

void OptimizeableSaliency_Output::clearCompressedCrops()
{
	auto compiter = resultCropsCompressed.begin();
	while(compiter != resultCropsCompressed.end()) {
		if(compiter->second != nullptr) {
			for(int ii=0; ii<compiter->second->size(); ii++) {
				delete compiter->second->at(ii);
			}
			compiter->second->clear();
			delete compiter->second;
		}
		compiter = resultCropsCompressed.erase(compiter);
	}
}

void OptimizeableSaliency_Output::CompressCropsSoFar()
{
	if(resultCrops.empty()==false) {
		std::vector<unsigned char> *newarr_writehere;
		std::vector<int> pngparam = std::vector<int>(2);
		pngparam[0] = CV_IMWRITE_PNG_COMPRESSION; pngparam[1] = 3; //0-9... 9 is smallest compressed size
		auto itimg = resultCrops.begin();
		while(itimg != resultCrops.end()) {
			if(itimg->second != nullptr) {
				CompressedCropsForImage(itimg->first); //add a slot in the map for this image, if it doesn't already exist
				if(itimg->second->empty()==false) {
					auto itcr = itimg->second->begin(); //iter for std::vector< cv::Mat >
					while(itcr != itimg->second->end()) {
						newarr_writehere = new std::vector<unsigned char>();
						cv::imencode(".png", *itcr, *newarr_writehere, pngparam);
						CompressedCropsForImage(itimg->first).push_back(newarr_writehere);
						itcr = itimg->second->erase(itcr);
					}
				}
			} else {
				cout<<"WARNING: INVALID IMAGE IN SALIENCY OUTPUT CROPS LIST"<<endl;
			}
			itimg = resultCrops.erase(itimg);
		}
	}
}

std::vector<cv::Mat>&
	OptimizeableSaliency_Output::CropsForImage(std::string imgFilename)
{
	if(resultCrops.find(imgFilename) == resultCrops.end()) {
		resultCrops[imgFilename] = new std::vector<cv::Mat>();
	}
	return *resultCrops[imgFilename];
}

std::vector<std::pair<double,double>>&
	OptimizeableSaliency_Output::LocationsForImage(std::string imgFilename)
{
	if(resultLocations.find(imgFilename) == resultLocations.end()) {
		resultLocations[imgFilename] = new std::vector<std::pair<double,double>>();
	}
	return *resultLocations[imgFilename];
}


std::vector<std::vector<unsigned char>*>&
	OptimizeableSaliency_Output::CompressedCropsForImage(std::string imgFilename)
{
	if(resultCropsCompressed.find(imgFilename) == resultCropsCompressed.end()) {
		resultCropsCompressed[imgFilename] = new std::vector<std::vector<unsigned char>*>();
	}
	return *resultCropsCompressed[imgFilename];
}


int OptimizeableSaliency_Output::NumCrops() const {
	int retval = 0;
	
	auto cropsiter = resultCrops.begin();
	while(cropsiter != resultCrops.end()) {
		if(cropsiter->second != nullptr) {
			retval += cropsiter->second->size();
		}
		cropsiter++;
	}
	auto compiter = resultCropsCompressed.begin();
	while(compiter != resultCropsCompressed.end()) {
		if(compiter->second != nullptr) {
			retval += compiter->second->size();
		}
		compiter++;
	}
	return retval;
}

int OptimizeableSaliency_Output::numFullsizeImages() const {
	if(resultCrops.size()!=resultLocations.size() && resultCropsCompressed.size()!=resultLocations.size()) {
		std::cout<<"WARNING IN SALIENCY_EXPERIMENT_RESULTS: INCONSISTENT SIZE"<<std::endl;
	}
	return resultLocations.size();
}

std::string OptimizeableSaliency_Output::ImgNameForIdx(int idx) {
	if(resultCrops.size()==resultLocations.size()) {
		if(idx >= 0 && idx < resultCrops.size()) {
			auto it = resultCrops.begin();
			if(idx == 0) return it->first;
			return std::next(it, idx)->first;
		}
	} else if(resultCropsCompressed.size()==resultLocations.size()) {
		if(idx >= 0 && idx < resultCropsCompressed.size()) {
			auto it = resultCropsCompressed.begin();
			if(idx == 0) return it->first;
			return std::next(it, idx)->first;
		}
	}
	std::cout<<"WARNING IN SALIENCY_EXPERIMENT_RESULTS: INCONSISTENT SIZE"<<std::endl;
	return "";
}

std::vector<cv::Mat>&
	OptimizeableSaliency_Output::CropsForIdx(int idx)
{
	if(idx >= 0 && idx < resultCrops.size()) {
		auto it = resultCrops.begin();
		if(idx == 0) return *(it->second);
		return *(std::next(it, idx)->second);
	}
	std::cout<<"FATAL ERROR IN SALIENCY EXPERIMENT RESULTS INDEXING FOR CROPS"<<std::endl;
	assert(false);
	return *(new std::vector<cv::Mat>());
}

std::vector<std::pair<double,double>>&
	OptimizeableSaliency_Output::LocationsForIdx(int idx)
{
	if(idx >= 0 && idx < resultLocations.size()) {
		auto it = resultLocations.begin();
		if(idx == 0) return *(it->second);
		return *(std::next(it, idx)->second);
	}
	std::cout<<"FATAL ERROR IN SALIENCY EXPERIMENT RESULTS INDEXING FOR LOCATIONS"<<std::endl;
	assert(false);
	return *(new std::vector<std::pair<double,double>>());
}

std::vector<std::vector<unsigned char>*>&
	OptimizeableSaliency_Output::CompressedCropsForIdx(int idx)
{
	if(idx >= 0 && idx < resultCropsCompressed.size()) {
		auto it = resultCropsCompressed.begin();
		if(idx == 0) return *(it->second);
		return *(std::next(it, idx)->second);
	}
	std::cout<<"FATAL ERROR IN SALIENCY EXPERIMENT RESULTS INDEXING FOR COMPRESSED CROPS"<<std::endl;
	assert(false);
	return *(new std::vector<std::vector<unsigned char>*>());
}
