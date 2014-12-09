#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

class ResultsData
{
public:
	int numFailures_DetectionFailures;
	int numFailures_PaddingFailures;
	int numFailures_TooBig;
	int numCloseCallsForCroppingFailures;
	int numSuccesses;
	int numFileErrors;
	int numDetectedThings;
	int numTruthTargets;
	
	ResultsData() :
		numFailures_DetectionFailures(0),
		numFailures_PaddingFailures(0),
		numFailures_TooBig(0),
		numCloseCallsForCroppingFailures(0),
		numSuccesses(0),
		numFileErrors(0),
		numDetectedThings(0),
		numTruthTargets(0) {}
};

ResultsData TestFolderWithCrops(std::string truthFilename, std::string truthImageFileFolder, std::string folderWithCrops,
						int desiredMinPaddingPixels, double desiredMaxCropLengthRatioToTargetLength,
						bool cropCoordinatesAreInFilename);
