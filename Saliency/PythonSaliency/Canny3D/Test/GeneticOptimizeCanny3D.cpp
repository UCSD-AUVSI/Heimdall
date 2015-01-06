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
#include "SharedUtils/optimization/experimentUtils.hpp"
#include "SharedUtils/optimization/MetropolisMonteCarlo.hpp"
#include "Saliency/ResultsTruthTester/optimizeSaliency.hpp"


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


class OptimizingState_C3D : public OptimizingSystemState_Saliency
{
public:
	OptimizingState_C3D() : OptimizingSystemState_Saliency() {InitArgs();}
	
	virtual void InitArgs()
	{
		OptimizedArgs.resize(4);
		OptimizedArgsUpdateSteps.resize(4);
		
		OptimizedArgs[0] = 101.558; //cThreshLow
		OptimizedArgs[1] = 85.8787; //cHighRatioTimes5
		OptimizedArgs[2] = 5.96391; //blurRadius
		OptimizedArgs[3] = 26.8199; //resizePct
		
		OptimizedArgsUpdateSteps[0] = 11.4;
		OptimizedArgsUpdateSteps[1] = 3.4;
		OptimizedArgsUpdateSteps[2] = 1.1;
		OptimizedArgsUpdateSteps[3] = 0.9;
	}
	virtual void ConstrainArgs()
	{		
		OptimizedArgs[0] = CLAMP(OptimizedArgs[0], 10.0, 1000.0);
		OptimizedArgs[1] = CLAMP(OptimizedArgs[1], 1.001, 200.0);
		OptimizedArgs[2] = CLAMP(OptimizedArgs[2], 3.0, 16.0);
		OptimizedArgs[3] = CLAMP(OptimizedArgs[3], 9.0, 31.0);
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
	
	std::vector<std::vector<unsigned char>*> pngImgs;
	std::vector<std::string> tImgFnames;
	loadImagesIntoCompressedMemory(argv[1], choose_a_random_training_subset_of_size,
									pngImgs, tImgFnames);
	
	cout << "found " << pngImgs.size() << " images to test with!" << endl;
	//exit(0);
	
//--------------------------------------------------------------------------------------------
	PythonSaliencyClass saldoer;
	saldoer.saliencyModuleFolderName = "Canny3D";
	saldoer.pythonFilename = "main.py";
	saldoer.pythonFunctionName = "doSaliency";
	
	const std::string truthFilename("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	const std::string folderForOutput("../../output_images");
	
	int maxLoopsBeforeQuitting = 200000000;
	OptimizingState_C3D previous;
	OptimizingState_C3D latest;
	
	
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

