#include <iostream>
using std::cout;
using std::endl;
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "Saliency/SpectralResidualSaliency/ProcessingClass.hpp"
#include "SharedUtils/GlobalVars.hpp"
#include <thread>
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include "SharedUtils/optimization/experimentUtils.hpp"
#include "Saliency/OptimizeableSaliency.hpp"
#include "SharedUtils/optimization/MetropolisMonteCarlo.hpp"
#include "SharedUtils/optimization/SimulatedAnnealing.hpp"


static int choose_a_random_training_subset_of_size = 500; //when > num images, will load all

static const std::string output_folder_for_final_results("../../output_images");




class OptimizingState_SRS : public OptimizeableSaliency_Params
{
	std::vector<double> paramsMins;
	std::vector<double> paramsMaxs;
public:
	OptimizingState_SRS() : OptimizeableSaliency_Params() {InitArgs();}
	
	virtual OptimizeableSaliency_Params* CreateInstance() {return new OptimizingState_SRS();}
	virtual void InitArgs()
	{
		SpectralSaliencyArgs test;
		test.GetVec(params);
		paramsStepSizes.resize(params.size());
		assert(params.size() == 8);
		paramsMins.resize(8);
		paramsMaxs.resize(8);
		
		double testspd = 1.1;
		paramsStepSizes[0] = 1.0 * testspd;
		paramsStepSizes[1] = 0.05 * testspd;
		paramsStepSizes[2] = 0.8 * testspd;
		paramsStepSizes[3] = 1.5 * testspd;
		paramsStepSizes[4] = 1.4 * testspd;
		paramsStepSizes[5] = 0.025 * testspd;
		paramsStepSizes[6] = 0.025 * testspd;
		paramsStepSizes[7] = 0.02 * testspd;
		
		paramsMins[0] = 30.0;	paramsMaxs[0] = 110.0;
		paramsMins[1] = 1.01;	paramsMaxs[1] = 6.0;
		paramsMins[2] = 4.0;	paramsMaxs[2] = 40.0;
		paramsMins[3] = 100.0;	paramsMaxs[3] = 500.0;
		paramsMins[4] = 15.0;	paramsMaxs[4] = 51.0;
		paramsMins[5] = 0.30;	paramsMaxs[5] = 0.60;
		paramsMins[6] = 0.10;	paramsMaxs[6] = 0.99;
		paramsMins[7] = 0.10;	paramsMaxs[7] = 0.70;
	}
	virtual void ConstrainArgs()
	{
		params[0] = CLAMP(params[0], 30.0, 110.0);
		params[1] = CLAMP(params[1], 1.01, 6.0);
		params[2] = CLAMP(params[2], 4.0, 40.0);
		params[3] = CLAMP(params[3], 100.0, 500.0);
		params[4] = CLAMP(params[4], 15.0, 51.0);
		params[5] = CLAMP(params[5], 0.30, 0.60);
		params[6] = CLAMP(params[6], 0.10, 0.99);
		params[7] = CLAMP(params[7], 0.10, 0.70);
	}
	
	virtual void GetArgConstraints(std::vector<double> const*& mins, std::vector<double> const*& maxs)
	{
		mins = &paramsMins;
		maxs = &paramsMaxs;
	}
};


int main(int argc, char** argv)
{
	cout << "Optimize Spectral Residual Saliency" << endl;
	if(argc < 2) {
		consoleOutput.Level0() << "usage:  [PATH TO FOLDER WITH IMAGES]" << endl;
		return 1;
	}
	if(!check_if_directory_exists(argv[1])) {
		consoleOutput.Level0() << "ERROR: path \""<<argv[1]<<"\" not found!!" << endl;
		return 1;
	}
	
	OptimizeableSaliency_SourceData srcImgDataClass;
	loadImagesIntoCompressedMemory(argv[1], choose_a_random_training_subset_of_size,
									srcImgDataClass.pngImgs, srcImgDataClass.tImgFnames);
	
	cout << "found " << srcImgDataClass.pngImgs.size() << " images to test with!" << endl;
	//exit(0);
	
//--------------------------------------------------------------------------------------------
	SpectralResidualSaliencyClass saldoer;
	saldoer.args.save_output_to_this_folder = output_folder_for_final_results;
	
	OptimizeableSaliency_TruthFilename = new std::string("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	OptimizeableSaliency_FolderToSaveOutput = new std::string("../../output_images");
	
	
	OptimizeableSaliency_Multithreaded mtSalOpt;
	mtSalOpt.actualSaliencyModule = &saldoer;
	mtSalOpt.paramsInstanceForCreatingCopies = new OptimizingState_SRS();
	
	//Optimizer_MCMC optimizer;
	SimulatedAnnealing optimizer;
	
	optimizer.InitialWarmup(&mtSalOpt, &srcImgDataClass);
	
	
	return 0;
}

