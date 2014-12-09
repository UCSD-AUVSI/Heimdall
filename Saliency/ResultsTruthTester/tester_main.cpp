#include <iostream>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "SaliencyResultsTruthTester.hpp"
#include "SharedUtils/SharedUtils.hpp"



int main(int argc, char** argv)
{
//======================================================================================
	int desiredMinPaddingPixels = 5;
	double desiredMaxCropLengthRatioToTargetLength = 3.5;
//======================================================================================
	
	if(argc < 2) {
		consoleOutput.Level0() << "usage:  [PATH TO FOLDER CONTAINING CROPPED OBJECT IMAGES]" << std::endl;
		return 1;
	}
	std::string folderWithCrops(argv[1]);
	if(folderWithCrops.empty()) {
		consoleOutput.Level0() << "error - folder is empty" << std::endl;
		return 1;
	}
	std::string truthImageFileFolder("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/");
	std::string truthFilename     (  "/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	
	bool CROP_COORDINATES_ARE_IN_FILENAMES = true;
	
	ResultsData results = TestFolderWithCrops(truthFilename, truthImageFileFolder, folderWithCrops,
												desiredMinPaddingPixels, desiredMaxCropLengthRatioToTargetLength,
												CROP_COORDINATES_ARE_IN_FILENAMES);
	
	std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "number of actual truth targets (from truth file): " << results.numTruthTargets << std::endl;
	std::cout << "num crops: " << results.numDetectedThings << std::endl;
	std::cout << "num successes: " << results.numSuccesses << std::endl;
	std::cout << "total num failures: " << (results.numFailures_DetectionFailures+results.numFailures_PaddingFailures) << std::endl;
	std::cout << "numCloseCallsForCroppingFailures: " << results.numCloseCallsForCroppingFailures << std::endl;
	std::cout << "num detection failures / false alarms: " << results.numFailures_DetectionFailures << std::endl;
	std::cout << "num padding failures: " << results.numFailures_PaddingFailures << std::endl;
	std::cout << "num file errors: " << results.numFileErrors << std::endl;
	//std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
}
