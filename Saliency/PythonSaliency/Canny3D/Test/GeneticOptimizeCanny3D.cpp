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
#include <thread>
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include "SharedUtils/optimization/experimentUtils.hpp"
#include "Saliency/OptimizeableSaliency.hpp"
#include "SharedUtils/optimization/MetropolisMonteCarlo.hpp"


static int choose_a_random_training_subset_of_size = 4; //DO ALL

static const std::string output_folder_for_final_results("../../output_images");



static std::string GetPathToTestExecutable(const char* argv0) {
	std::string fullpath = GetPathOfExecutable(argv0);
	std::string trimThis("/bin/test/testGeneticOptimizeCanny3D");
	if(fullpath.substr(fullpath.size()-trimThis.size()) != trimThis) {
		std::cout<<"WARNING: TEST EXECUTABLE NOT FOUND ???? error finding path to executable"<<std::endl;
	}
	fullpath.erase(fullpath.size()-trimThis.size());
	return fullpath;
}


class OptimizingState_C3D : public OptimizeableSaliency_Params
{
public:
	OptimizingState_C3D() : OptimizeableSaliency_Params() {InitArgs();}
	
	virtual OptimizeableSaliency_Params* CreateInstance() {return new OptimizingState_C3D();}
	virtual void InitArgs()
	{
		params.resize(4);
		paramsStepSizes.resize(4);
		
		params[0] = 101.558; //cThreshLow
		params[1] = 85.8787; //cHighRatioTimes5
		params[2] = 5.96391; //blurRadius
		params[3] = 26.8199; //resizePct
		
		paramsStepSizes[0] = 11.4;
		paramsStepSizes[1] = 3.4;
		paramsStepSizes[2] = 1.1;
		paramsStepSizes[3] = 0.9;
	}
	virtual void ConstrainArgs()
	{		
		params[0] = CLAMP(params[0], 10.0, 1000.0);
		params[1] = CLAMP(params[1], 1.001, 200.0);
		params[2] = CLAMP(params[2], 3.0, 16.0);
		params[3] = CLAMP(params[3], 9.0, 31.0);
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
	
	OptimizeableSaliency_SourceData srcImgDataClass;
	loadImagesIntoCompressedMemory(argv[1], choose_a_random_training_subset_of_size,
									srcImgDataClass.pngImgs, srcImgDataClass.tImgFnames);
	
	cout << "found " << srcImgDataClass.pngImgs.size() << " images to test with!" << endl;
	//exit(0);
	
//--------------------------------------------------------------------------------------------
	PythonSaliencyClass saldoer;
	saldoer.saliencyModuleFolderName = "Canny3D";
	saldoer.pythonFilename = "main.py";
	saldoer.pythonFunctionName = "doSaliency";
	
	OptimizeableSaliency_TruthFilename = new std::string("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	OptimizeableSaliency_FolderToSaveOutput = new std::string("../../output_images");
	
	
	OptimizeableSaliency_Multithreaded mtSalOpt;
	mtSalOpt.actualSaliencyModule = &saldoer;
	mtSalOpt.paramsInstanceForCreatingCopies = new OptimizingState_C3D();
	
	Optimizer_MCMC optimizer;
	optimizer.InitialWarmup(&mtSalOpt, &srcImgDataClass);
	
	
	return 0;
}

