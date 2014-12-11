#ifndef __PYTHON_SALIENCY_HPP____
#define __PYTHON_SALIENCY_HPP____

#include <string>
#include <opencv2/highgui/highgui.hpp>

void pythonSaliency(std::string saliencyModuleFolderName,
					std::string pythonFilename,
					std::string pythonFunctionName,
					cv::Mat fullsizeImage,
					std::vector<cv::Mat> & returnedCrops,
					std::vector<std::pair<double,double>> & returned_geolocations,
					std::vector<double> *additional_args=nullptr);

#endif
