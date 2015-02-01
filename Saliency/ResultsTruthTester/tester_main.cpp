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
	std::string truthImageFileFolder("/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth");
	std::string truthFilename     (  "/media/C:/LinuxShared/AUVSI/2014-2015-train-with-truth/Truth2013.txt");
	(*OptimizeableSaliency_TruthFilename) = truthFilename;
	
	bool CROP_COORDINATES_ARE_IN_FILENAMES = true;
	
	OptimizeableSaliency_Output output = ReadFolderWithCrops(truthImageFileFolder, folderWithCrops,
												CROP_COORDINATES_ARE_IN_FILENAMES);
	
	OptimizeableSaliency_ResultsStats * results = dynamic_cast<OptimizeableSaliency_ResultsStats*>(output.CalculateResults());
	
	results->Print(std::cout, true);
}
