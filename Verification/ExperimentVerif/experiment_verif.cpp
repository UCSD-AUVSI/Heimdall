#include <string>
#include <iostream>

#include "Verification/ExperimentVerif/experiment_verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "opencv2/opencv.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp" //for saveImage
using std::cout;
using std::endl;

std::map<std::string,std::pair<int,int>> receivedImages;
std::vector<imgdata_t*> receivedResults;


const bool kSaveCropsWithImageNames = true;
const bool kSaveImages = true;
const bool kSaveSegs = false;
const bool kSaveCrops_OnlyWhenReported = true;

const bool kShowImages = false;
const bool kShowIfSsegCsegSuccess = false;

const std::string output_folder("../../output_images");


void ExperimentVerify :: execute(imgdata_t *imdata, std::string args){
    cout << "ExperimentVerify, ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;

    bool both_cseg_and_sseg_succeeded = (imdata->sseg_image_data->empty()==false && imdata->sseg_image_data->empty()==false);
	
	
	if(imdata->name_of_original_image_file_for_debugging.empty() == false)
	{
		imgdata_t *dataCopy = new imgdata_t();
		copyIMGData(dataCopy, imdata);
		receivedResults.push_back(dataCopy);
		
		
		std::map<std::string,std::pair<int,int>>::iterator imgiter = receivedImages.find(imdata->name_of_original_image_file_for_debugging);
		if(imgiter == receivedImages.end()) {
			receivedImages[imdata->name_of_original_image_file_for_debugging]
						= std::pair<int,int>(1,imdata->num_crops_in_this_image);
		}
		else {
			receivedImages[imdata->name_of_original_image_file_for_debugging].first += 1;
		}
		
		cout<<"map<\""<<imdata->name_of_original_image_file_for_debugging<<
			"\"> == ("<<to_istring(receivedImages[imdata->name_of_original_image_file_for_debugging].first)
			<<","<<to_istring(receivedImages[imdata->name_of_original_image_file_for_debugging].second)<<")"<<endl;
		
		if(receivedImages.size() >= globalNumImagesInExperiment) {
			int images_that_are_finished = 0;
			for(imgiter = receivedImages.begin(); imgiter != receivedImages.end(); imgiter++) {
				if(imgiter->second.first == imgiter->second.second) {
					images_that_are_finished++;
				}
			}
			if(images_that_are_finished == globalNumImagesInExperiment) {
				cout<<"================================================================"
					<<endl<<"EXPERIMENTVERIFY: DONE PROCESSING EVERYTHING"<<endl;
				
				if(globalExperimentResultsCalculatorFunc != nullptr) {
					globalExperimentResultsCalculatorFunc(receivedResults, globalLatestExperimentResults);
				}
				
				//start another experiment
				globalExperimentNum++;
			}
		}
	}
	
#if 0
    // Save images and crops
    std::string name_of_input_crop = std::to_string(imdata->id) + "_" + std::to_string(imdata->cropid);
    if(kSaveImages && check_if_directory_exists(output_folder)) {
        if(imdata->image_data->size()){
			if(kSaveCrops_OnlyWhenReported==false || both_cseg_and_sseg_succeeded) {
				cv::Mat image = cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR);
				if(kSaveCropsWithImageNames == false) {
				saveImage(image, output_folder + "/" + name_of_input_crop + "___SSEG_" + to_sstring(imdata->scolor) + "___CSEG_" + to_sstring(imdata->ccolor)
								+ "_" + to_istring(imdata->targetlat) + ",,," + to_istring(imdata->targetlongt) + "_.jpg");
				} else {
				saveImage(image, output_folder + "/" + imdata->name_of_original_image_file_for_debugging + "_crop" + to_istring(imdata->cropid) + "_" + to_istring(imdata->targetlat) + ",,," + to_istring(imdata->targetlongt) + "_.jpg");
				}
			}
        }
        if(kSaveSegs){   
            int count = 0;
            for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
                    i < imdata->sseg_image_data->end(); ++i){
                cv::Mat image = cv::imdecode(**i, CV_LOAD_IMAGE_COLOR);
                saveImage(image, output_folder + "/" + name_of_input_crop + "_" + std::to_string(count++) + "__SSEG.png");
            }
            count = 0;
            for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
                    i < imdata->cseg_image_data->end(); ++i){
                cv::Mat image = cv::imdecode(**i, CV_LOAD_IMAGE_COLOR);
                saveImage(image, output_folder + "/" + name_of_input_crop + "_" + std::to_string(count++) + "__CSEG.png");
            }
        }
    }
#endif


    // Verification check, do we want this message to continue?
    if (!both_cseg_and_sseg_succeeded || imdata->shape == "") {
        clearIMGData(imdata);
    } else {
        setDone(imdata, VERIF);
    }
}
