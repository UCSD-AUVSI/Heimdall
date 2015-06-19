#include <string>
#include <iostream>

#include "Verification/DisplayVerif/display_verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "opencv2/opencv.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp" //for saveImage

using std::cout;
using std::endl;

const bool kSaveCropsWithImageNames = true;
const bool kSaveImages = true;
const bool kSaveSegs = true;
const bool kSaveCrops_OnlyWhenReported = true;

const bool kShowImages = false;
const bool kShowIfSsegCsegSuccess = false;

const std::string output_folder("../../output_images");

std::fstream* outfile_verif_results = nullptr;
bool outfile_verif_results_has_been_opened = false;

void DisplayVerify :: execute(imgdata_t *imdata, std::string args){
    cout << "DisplayVerify , ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;

    bool both_cseg_and_sseg_succeeded = (imdata->sseg_image_data->empty()==false && imdata->sseg_image_data->empty()==false);
    cout << "CS: " << (both_cseg_and_sseg_succeeded?"true":"false") << endl;
    bool save_output_file = true;

    // Update results file
    if(outfile_verif_results == nullptr) {
        if(outfile_verif_results_has_been_opened == false) {
            outfile_verif_results = new std::fstream();
            save_output_file = save_output_file?(check_if_directory_exists(output_folder) && outfile_verif_results!=nullptr):false;
            if(save_output_file) {
                outfile_verif_results_has_been_opened = true;
                outfile_verif_results->open(output_folder+std::string("/verif_results.txt"),
                        std::fstream::trunc | std::fstream::out); //clear the file of data (if it existed): we're about to rewrite it
                save_output_file = save_output_file?(outfile_verif_results->is_open() && outfile_verif_results->good()):false;
            }
        }
    }
    else {
        save_output_file = save_output_file?(outfile_verif_results->is_open() && outfile_verif_results->good()):false;
    }
    if(both_cseg_and_sseg_succeeded && save_output_file) {
        cout << "==============================wrote the below results to file" << endl;
        (*outfile_verif_results) << "--------------------------------" << endl;
        (*outfile_verif_results) << "target# " << imdata->id << ", " << imdata->cropid << endl;
        (*outfile_verif_results) << "shape:  " << imdata->shape << endl;
        (*outfile_verif_results) << "scolor:  " << imdata->scolor << endl;
        (*outfile_verif_results) << "char:  " << imdata->character1 << endl;
        (*outfile_verif_results) << "char2:  " << imdata->character2 << endl;
        (*outfile_verif_results) << "ccolor:  " << imdata->ccolor << endl;
        (*outfile_verif_results) << "lat:  " << imdata->targetlat << endl;
        (*outfile_verif_results) << "long:  " << imdata->targetlongt << endl;
        (*outfile_verif_results) << std::flush;
    }

    // Save images and crops
    std::string name_of_input_crop = std::to_string(imdata->id) + "_" + std::to_string(imdata->cropid);
    if(kSaveImages && check_if_directory_exists(output_folder)) {
        if(imdata->image_data->size()){
			if(kSaveCrops_OnlyWhenReported==false || both_cseg_and_sseg_succeeded) {
				cv::Mat image = cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR);
				char latlongstr[1024];
				sprintf(latlongstr, "%lf,,,%lf_.jpg", imdata->targetlat, imdata->targetlongt);
				if(kSaveCropsWithImageNames == false) {
				saveImage(image, output_folder + "/" + name_of_input_crop + "___SSEG_" + to_sstring(imdata->scolor) + "___CSEG_" + to_sstring(imdata->ccolor)+ "_" + std::string(latlongstr));
				} else {
				saveImage(image, output_folder + "/" + imdata->name_of_original_image_file_for_debugging + "_crop" + to_istring(imdata->cropid) + "_" + std::string(latlongstr));
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

    // Give some information to cout
    if(imdata->shape.empty())
        cout << "no shape found!" << endl;
    else
        cout << "shape found: \'" << imdata->shape << "\'" << endl;

    if(imdata->character1.empty())
        cout << "no character (1) found!" << endl;
    else
        cout << "character (1) found: \'" << imdata->character1 << "\'" << endl;

    if(imdata->character2.empty())
        cout << "no character 2" << endl;
    else
        cout << "character 2 found: \'" << imdata->character2 << "\'" << endl;

    // Show images
    if(kShowImages)
    {
        std::string name_of_crop_window = std::string("crop (") + to_istring(imdata->id)
            + std::string(",") + to_istring(imdata->cropid) + std::string("), char \'")
            + imdata->character1 + std::string("\'");

        cv::startWindowThread();
        cv::namedWindow(name_of_crop_window, CV_WINDOW_NORMAL);

        cv::imshow(name_of_crop_window,	cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR));
        cv::waitKey(0);

        if(!kShowIfSsegCsegSuccess || (!imdata->sseg_image_data->empty() && !imdata->sseg_image_data->empty()))
        {
            for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
                    i < imdata->sseg_image_data->end(); ++i){
                cv::imshow(name_of_crop_window,	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
                cv::waitKey(0);
            }

            for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
                    i < imdata->cseg_image_data->end(); ++i){
                cv::imshow(name_of_crop_window,	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
                cv::waitKey(0);
            }
        }
        cv::destroyWindow(name_of_crop_window);
    }

    // Verification check, do we want this message to continue?
    if (!both_cseg_and_sseg_succeeded || imdata->shape == "") {
		consoleOutput.Level1()<<"image ("<<imdata->id<<","<<imdata->cropid<<") not verified, probably rejected by CSEG, SSEG, or shape rec!"<<endl;
        clearIMGData(imdata);
    } else {
        setDone(imdata, VERIF);
    }
}
