#ifndef __GENERIC_SALIENCY_IMPLEMENTATION_HPP__
#define __GENERIC_SALIENCY_IMPLEMENTATION_HPP__

#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include "SharedUtils/optimization/OptimizeableModule.hpp"
class OptimizeableSaliency_ResultsStats;

extern std::string * OptimizeableSaliency_TruthFilename;
extern std::string * OptimizeableSaliency_FolderToSaveOutput;


/*----------------------------------------------------
	1. Data holder - e.g. images
*/
class OptimizeableSaliency_SourceData : public Optimizer_SourceData
{
public:
	std::vector<std::vector<unsigned char>*> pngImgs;
	std::vector<std::string> tImgFnames;
};


/*----------------------------------------------------
	2. Parameters (arguments) for module
*/
class OptimizeableSaliency_Params : public Optimizer_Params
{
public:
	std::vector<double> params;
	std::vector<double> paramsStepSizes;
	
	virtual void CopyFromOther(Optimizer_Params const*const other);
	virtual void Print(std::ostream & printHere);
	virtual void GenerateNewArgs(double arg);
	
	/* Implement these!*/
	//virtual void InitArgs() = 0;
	virtual void ConstrainArgs() = 0;
	virtual OptimizeableSaliency_Params* CreateInstance() = 0;
};


/*----------------------------------------------------
	3. Output data holder - e.g. cropped images, shape names, colors, etc.
*/
class OptimizeableSaliency_Output : public Optimizer_Optimizee_Output
{
public:
	virtual Optimizer_ResultsStats * CalculateResults();
	virtual void SaveToDisk();
	
	virtual void clear();
	virtual ~OptimizeableSaliency_Output() {clear();}
	
	/*
		Maps from original-image-filename to the results Saliency got from them
		
		Crops format:	std::vector< cv::Mat >
		
		Locations format: std::vector< std::pair<double,double> >
	*/
	std::map<std::string, std::vector<std::vector<unsigned char>*>*> resultCropsCompressed;
	std::map<std::string, std::vector<cv::Mat>*> resultCrops;
	std::map<std::string, std::vector<std::pair<double,double>>*> resultLocations;
	
	/*
		Get results by source image filename
	*/
	std::vector<cv::Mat>&
		CropsForImage(std::string imgFilename);
	
	std::vector<std::pair<double,double>>&
		LocationsForImage(std::string imgFilename);
	
	std::vector<std::vector<unsigned char>*>&
		CompressedCropsForImage(std::string imgFilename);
	
	/*
		Compress crops to save memory
	*/
	void clearCompressedCrops();
	void CompressCropsSoFar();
	
	/*
		Get results by the index when looping over all source images
	*/
	int NumCrops() const;
	int numFullsizeImages() const;
	
	std::string ImgNameForIdx(int idx);
	
	std::vector<cv::Mat>&
		CropsForIdx(int idx);
	
	std::vector<std::pair<double,double>>&
		LocationsForIdx(int idx);
		
	std::vector<std::vector<unsigned char>*>&
		CompressedCropsForIdx(int idx);
};


/*----------------------------------------------------
	4. Results stats holder - how good was the output?
*/
class OptimizeableSaliency_ResultsStats : public Optimizer_ResultsStats
{
public:
	virtual void CopyFromOther(Optimizer_ResultsStats const*const other);
	virtual double CalculateFitnessScore();
	virtual void Print(std::ostream & printHere, bool more_detailed);
//-------------------------------------------------------------------------
static int DesiredTarget_MinPaddingPixels;
static int DesiredFalseP_MinPaddingPixels;
static double DesiredTarget_MaxCropLengthRatioToTargetLength; //for CloseFailures_FarTooBig
static double DesiredFalseP_MaxCropLengthRatioToTargetLength; //for CloseFailures_FarTooBig
	
	std::vector<std::string> DesiredTargets_Missed_IncludingCloseFailures;
	std::vector<std::string> DesiredFalsePs_Missed_IncludingCloseFailures;
	
	int Truth_DesiredTargets;
	int Truth_DesiredFalsePs;
	
	int DesiredTarget_Successes;
	int DesiredFalseP_Successes;
	
	int DesiredTarget_CloseFailures_MinorPadding;
	int DesiredFalseP_CloseFailures_MinorPadding;
	
	int DesiredTarget_CloseFailures_FarTooBig;
	int DesiredFalseP_CloseFailures_FarTooBig;
	
	int DesiredTarget_CloseFailures_PartialCrop;
	int DesiredFalseP_CloseFailures_PartialCrop;
	
	int Total_NothingUsefulFailures;
	int Total_Detections;
	
	OptimizeableSaliency_ResultsStats() :
		Truth_DesiredTargets(0),
		Truth_DesiredFalsePs(0),
		DesiredTarget_Successes(0),
		DesiredFalseP_Successes(0),
		DesiredTarget_CloseFailures_MinorPadding(0),
		DesiredFalseP_CloseFailures_MinorPadding(0),
		DesiredTarget_CloseFailures_FarTooBig(0),
		DesiredFalseP_CloseFailures_FarTooBig(0),
		DesiredTarget_CloseFailures_PartialCrop(0),
		DesiredFalseP_CloseFailures_PartialCrop(0),
		Total_NothingUsefulFailures(0),
		Total_Detections(0) {}
	
	int NumCloseErrors() {return (DesiredTarget_CloseFailures_MinorPadding+DesiredFalseP_CloseFailures_MinorPadding+DesiredTarget_CloseFailures_FarTooBig+DesiredFalseP_CloseFailures_FarTooBig+DesiredTarget_CloseFailures_PartialCrop+DesiredFalseP_CloseFailures_PartialCrop);}
	int TruePositives() {return (DesiredTarget_Successes+DesiredFalseP_Successes);}
	int FalsePositives() {return Total_NothingUsefulFailures;}
	int FalseNegatives() {if((Truth_DesiredTargets+Truth_DesiredFalsePs)<=0){return 0;} return (Truth_DesiredTargets+Truth_DesiredFalsePs - TruePositives() - NumCloseErrors());}
	
};


/*-----------------------------------------------------------------------
	Implementations of actual Saliency modules should inherit from this
*/
class GenericSaliencyImplementation
{
public:
	virtual void ReceivedUpdatedArgs(const std::vector<double> & newArgs) = 0;
	
	virtual void ProcessSaliency(cv::Mat * fullsizeImage,
								std::vector<cv::Mat> * returnedCrops,
								std::vector<std::pair<double,double>> * returned_geolocations,
								int threadNumForDebugging) = 0;
};


/*----------------------------------------------------
	5. the module being optimized (Saliency, Segmentation, etc...)
*/
class OptimizeableSaliency_Multithreaded : public Optimizer_Optimizee
{
public:
	GenericSaliencyImplementation * actualSaliencyModule;
	OptimizeableSaliency_Params * paramsInstanceForCreatingCopies;
	
	virtual Optimizer_Optimizee_Output * CreateOutput() {return new OptimizeableSaliency_Output();}
	virtual Optimizer_ResultsStats * CreateResultsStats() {return new OptimizeableSaliency_ResultsStats();}
	
	OptimizeableSaliency_Multithreaded() : actualSaliencyModule(nullptr), paramsInstanceForCreatingCopies(nullptr) {}
	
	virtual void ProcessData(Optimizer_SourceData * givenData, Optimizer_Optimizee_Output * returnedOutput);
	
	virtual Optimizer_Params * CreateNewParams();
	virtual void ReceivedUpdatedArgs(Optimizer_Params const*const newArgs);
};



#endif
