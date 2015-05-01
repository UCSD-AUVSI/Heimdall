/**
 * TruthViewer
 * Author: Jason Bunk
 * Viewer for truth data; shows training images and boxes around targets.
 * Can be used to cut out and save crops around targets.
 *     Saves these crops to "../../output_images" which if run from the /Heimdall/build directory,
 *     then saves to "output_images" which should be adjacent to the "Heimdall" folder
**/

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "TruthFile.hpp"
#include <iostream>
using std::cout; using std::endl;


int main(int argc, char** argv)
{
	std::string filename_of_truth;
	std::string folder_dir_of_images;
	bool saveCrops = false;
	bool showDisplayImages = true;
	int cropPaddingPixels = 0;
	
	if(argc >= 4) {
		filename_of_truth = std::string(argv[1]);
		folder_dir_of_images = std::string(argv[2]);
		
		if(folder_dir_of_images[folder_dir_of_images.size()-1] != '/') {
			folder_dir_of_images = (folder_dir_of_images + std::string("/"));
		}
		
		if(std::string(argv[3]) == "0" || std::string(argv[3]) == "1") {
			saveCrops = (atoi(argv[3]) == 1);
		}
		else {
			std::cout << "usage:  [TRUTH FILE]  [IMAGES DIR]  [SAVE CROPS? BOOL 1/0]  OPTIONAL:[CROP PADDING PIXELS]  OPTIONAL:[DISPLAY IMAGES?]" << std::endl;
			return 1;
		}
		if(argc >= 5) {
			cropPaddingPixels = atoi(argv[4]);
		}
		if(argc >= 6) {
			showDisplayImages = (atoi(argv[5]) != 0);
		}
	}
	else {
		cout << "usage:  [TRUTH FILE]  [IMAGES DIR]  [SAVE CROPS? BOOL 1/0]  OPTIONAL:[CROP PADDING PIXELS]  OPTIONAL:[DISPLAY IMAGES?]" << std::endl;
		return 0;
	}	
	if(check_if_file_exists(filename_of_truth)==false) {
		cout << "could not find truth file" << std::endl;
		return 1;
	}
	if(check_if_directory_exists(folder_dir_of_images)==false) {
		cout << "could not find images directory" << std::endl;
		return 1;
	}
	
	TruthFile loadedFile;
	if(LoadTruthFile(filename_of_truth, loadedFile) == false) {
		return 1;
	}
	
	
	for(int i=0; i<loadedFile.images.size(); i++) {
		cv::Mat loadedImage = cv::imread(folder_dir_of_images + loadedFile.images[i].image_file);
		if(loadedImage.empty()) {
			std::cout << "warning: could not load image file \"" << loadedFile.images[i].image_file << "\"!" << std::endl;
		}
		else {
			std::cout << "Image: " << loadedFile.images[i].image_file << std::endl;
			for(int t=0; t<loadedFile.images[i].targets_in_image.size(); t++) {
				cv::Rect boxToDraw;
				cv::Rect boxToCrop;
				int readxx, readyy;
				std::string shapename;
				
				boxToCrop.x = readxx = atoi(GetTruthEntryValue("pos_x", loadedFile.images[i].targets_in_image[t]).c_str());
				boxToCrop.x -= cropPaddingPixels;
				boxToDraw.x = boxToCrop.x - 4; //accomodate for line widths
			
				boxToCrop.y = readyy = atoi(GetTruthEntryValue("pos_y", loadedFile.images[i].targets_in_image[t]).c_str());
				boxToCrop.y -= cropPaddingPixels;
				boxToDraw.y = boxToCrop.y - 4; //accomodate for line widths
				
				boxToCrop.width = atoi(GetTruthEntryValue("box_min_width", loadedFile.images[i].targets_in_image[t]).c_str());
				boxToCrop.width += (2*cropPaddingPixels);
				boxToDraw.width = boxToCrop.width + 8; //accomodate for line widths
				
				boxToCrop.height = atoi(GetTruthEntryValue("box_min_height", loadedFile.images[i].targets_in_image[t]).c_str());
				boxToCrop.height += (2*cropPaddingPixels);
				boxToDraw.height = boxToCrop.height + 8; //accomodate for line widths
				
				//make sure boxToCrop doesnt go outside the image bounds
				if(boxToCrop.x < 0) boxToCrop.x = 0;
				if(boxToCrop.y < 0) boxToCrop.y = 0;
				if(boxToCrop.x+boxToCrop.width >= loadedImage.cols){boxToCrop.width = (loadedImage.cols - boxToCrop.x - 1);}
				if(boxToCrop.y+boxToCrop.height >= loadedImage.rows){boxToCrop.height = (loadedImage.rows - boxToCrop.y - 1);}
				
				shapename = GetTruthEntryValue("shape", loadedFile.images[i].targets_in_image[t]);
				
				std::cout << "   Target: " << shapename << " at (x,y) == (" << readxx << "," << readyy << ")" << std::endl;
				
				if(saveCrops) {
					cv::imwrite(std::string("../../output_images/")+loadedFile.images[i].image_file.substr(0,loadedFile.images[i].image_file.size()-4)+std::string("_")+shapename+std::string("_")+to_istring(t)+std::string(".png"), loadedImage(boxToCrop));
				}
				if(showDisplayImages) {
					cv::rectangle(loadedImage, boxToDraw, cv::Scalar(127,0,255), 5, 4);
				}
			}
			for(int t=0; t<loadedFile.images[i].falsepositives_in_image.size(); t++) {
				std::cout << "   FalsePositive:" << std::endl;
				cv::Rect boxToDraw;
				cv::Rect boxToCrop;
				
				boxToCrop.x = atoi(GetTruthEntryValue("pos_x", loadedFile.images[i].falsepositives_in_image[t]).c_str());
				boxToCrop.x -= cropPaddingPixels;
				boxToDraw.x = boxToCrop.x - 4; //accomodate for line widths
			
				boxToCrop.y = atoi(GetTruthEntryValue("pos_y", loadedFile.images[i].falsepositives_in_image[t]).c_str());
				boxToCrop.y -= cropPaddingPixels;
				boxToDraw.y = boxToCrop.y - 4; //accomodate for line widths
				
				boxToCrop.width = atoi(GetTruthEntryValue("box_min_width", loadedFile.images[i].falsepositives_in_image[t]).c_str());
				boxToCrop.width += (2*cropPaddingPixels);
				boxToDraw.width = boxToCrop.width + 8; //accomodate for line widths
				
				boxToCrop.height = atoi(GetTruthEntryValue("box_min_height", loadedFile.images[i].falsepositives_in_image[t]).c_str());
				boxToCrop.height += (2*cropPaddingPixels);
				boxToDraw.height = boxToCrop.height + 8; //accomodate for line widths
				
				//make sure boxToCrop doesnt go outside the image bounds
				if(boxToCrop.x < 0) boxToCrop.x = 0;
				if(boxToCrop.y < 0) boxToCrop.y = 0;
				if(boxToCrop.x+boxToCrop.width >= loadedImage.cols){boxToCrop.width = (loadedImage.cols - boxToCrop.x - 1);}
				if(boxToCrop.y+boxToCrop.height >= loadedImage.rows){boxToCrop.height = (loadedImage.rows - boxToCrop.y - 1);}
				
				if(saveCrops) {
					cv::imwrite(std::string("../../output_images/")+loadedFile.images[i].image_file.substr(0,loadedFile.images[i].image_file.size()-4)+std::string("_falsepositive_")+to_istring(t)+std::string(".png"), loadedImage(boxToCrop));
				}
				if(showDisplayImages) {
					cv::rectangle(loadedImage, boxToDraw, cv::Scalar(200,0,255), 5, 4);
				}
			}
			
			if(showDisplayImages) {
				double maxDispWidth = 1300;
				double maxDispHeight = 700;
				if(loadedImage.rows > maxDispHeight || loadedImage.cols > maxDispWidth) {
					double scaleFactor = std::min(maxDispWidth/((double)loadedImage.cols), maxDispHeight/((double)loadedImage.rows));
					cv::resize(loadedImage, loadedImage, cv::Size(0.0,0.0), scaleFactor, scaleFactor);
				}
				double minDispWidth = 40;
				double minDispHeight = 40;
				if(loadedImage.rows < minDispHeight || loadedImage.cols < minDispWidth) {
					double scaleFactor = std::min(minDispWidth/((double)loadedImage.cols), minDispHeight/((double)loadedImage.rows));
					cv::resize(loadedImage, loadedImage, cv::Size(0.0,0.0), scaleFactor, scaleFactor);
				}
				cv::imshow("image",loadedImage);
				cv::waitKey(0);
				cv::destroyAllWindows();
			}
		}
	}
	
	return 0;
}
