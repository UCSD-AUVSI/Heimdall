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
#include "Saliency/ResultsTruthTester/SaliencyExperimentResults.hpp"
#include <thread>
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include "SharedUtils/optimization/experimentUtils.hpp"
#include "SharedUtils/optimization/MetropolisMonteCarlo.hpp"
#include "Saliency/ResultsTruthTester/optimizeSaliency.hpp"


static int choose_a_random_training_subset_of_size = 40; //DO ALL

static const std::string output_folder_for_final_results("../../output_images");




class OptimizingState_SRS : public OptimizingSystemState_Saliency
{
public:
	OptimizingState_SRS() : OptimizingSystemState_Saliency() {InitArgs();}
	
	virtual void InitArgs()
	{
		SpectralSaliencyArgs test;
		test.GetVec(OptimizedArgs);
		OptimizedArgsUpdateSteps.resize(OptimizedArgs.size());
		assert(OptimizedArgs.size() == 8);
		
		double testspd = 1.1;
		OptimizedArgsUpdateSteps[0] = 1.0 * testspd;
		OptimizedArgsUpdateSteps[1] = 0.05 * testspd;
		OptimizedArgsUpdateSteps[2] = 0.8 * testspd;
		OptimizedArgsUpdateSteps[3] = 1.5 * testspd;
		OptimizedArgsUpdateSteps[4] = 1.4 * testspd;
		OptimizedArgsUpdateSteps[5] = 0.025 * testspd;
		OptimizedArgsUpdateSteps[6] = 0.025 * testspd;
		OptimizedArgsUpdateSteps[7] = 0.02 * testspd;
	}
	virtual void ConstrainArgs()
	{
		OptimizedArgs[0] = CLAMP(OptimizedArgs[0], 30.0, 110.0);
		OptimizedArgs[1] = CLAMP(OptimizedArgs[1], 1.01, 6.0);
		OptimizedArgs[2] = CLAMP(OptimizedArgs[2], 4.0, 40.0);
		OptimizedArgs[3] = CLAMP(OptimizedArgs[3], 100.0, 500.0);
		OptimizedArgs[4] = CLAMP(OptimizedArgs[4], 15.0, 51.0);
		OptimizedArgs[5] = CLAMP(OptimizedArgs[5], 0.30, 0.60);
		OptimizedArgs[6] = CLAMP(OptimizedArgs[6], 0.10, 0.99);
		OptimizedArgs[7] = CLAMP(OptimizedArgs[7], 0.10, 0.70);
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
	
	std::vector<std::vector<unsigned char>*> pngImgs;
	std::vector<std::string> tImgFnames;
	loadImagesIntoCompressedMemory(argv[1], choose_a_random_training_subset_of_size,
									pngImgs, tImgFnames);
	
	cout << "found " << pngImgs.size() << " images to test with!" << endl;
	//exit(0);
	
//--------------------------------------------------------------------------------------------
	//SaliencyExperimentResults::desiredMinPaddingPixels = 5;
	//SaliencyExperimentResults::desiredMaxCropLengthRatioToTargetLength = 3.5;
//--------------------------------------------------------------------------------------------
	SpectralResidualSaliencyClass saldoer;
	saldoer.args.save_output_to_this_folder = output_folder_for_final_results;
	
	const std::string truthFilename("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	const std::string folderForOutput("../../output_images");
	
	int maxLoopsBeforeQuitting = 3000000;
	OptimizingState_SRS previous;
	OptimizingState_SRS latest;
	
	StartOptimizationLoop(&saldoer,
							1.0,
							&previous,
							&latest,
							pngImgs,
							tImgFnames,
							maxLoopsBeforeQuitting,
							truthFilename,
							folderForOutput);
	
	return 0;
}

