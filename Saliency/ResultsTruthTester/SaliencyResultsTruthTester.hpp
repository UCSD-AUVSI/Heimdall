#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include "SaliencyExperimentResults.hpp"
#include "Saliency/OptimizeableSaliency.hpp"


OptimizeableSaliency_Output
	ReadFolderWithCrops(std::string truthImageFileFolder,
						std::string folderWithCrops,
						bool cropCoordinatesAreInFilename,
						bool compress=true);


/*#ifndef imgdata_t
class imgdata_t;
#endif
#ifndef ExperimentResultsData
class ExperimentResultsData;
#endif
void SaliencyExperimentResultsCalculator(std::vector<imgdata_t*> imgResults,
										ExperimentResultsData* calculatedResults);
*/

