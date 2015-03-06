#ifndef __GLOBAL_VARS_HPP__
#define __GLOBAL_VARS_HPP__

// These extern variables will be defined in "SharedUtils.cpp"


extern std::string* path_to_HeimdallBuild_directory;
extern std::string* path_to_HClient_executable;



class ExperimentResultsData
{
public:
	int NumTruePositives;
	int NumTrueNegatives;
	int NumFalsePositives;
	int NumFalseNegatives;
	
	ExperimentResultsData() :
			NumTruePositives(0),
			NumTrueNegatives(0),
			NumFalsePositives(0),
			NumFalseNegatives(0) {}
};


extern ExperimentResultsData* globalLatestExperimentResults;

extern int globalNumImagesInExperiment;
extern int globalExperimentNum;

#ifndef imgdata_t
class imgdata_t;
#endif
extern void (*globalExperimentResultsCalculatorFunc)(std::vector<imgdata_t*>, ExperimentResultsData*);




#endif
