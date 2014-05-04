#include <string>
#include <iostream>

#include "Verification/DisplayVerif/display_verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "opencv2/opencv.hpp"
#include "SharedUtils/SharedUtils.hpp"

using std::cout;
using std::endl;

const bool kShowImages = false;
const bool kShowIfSsegCsegSuccess = true;

std::fstream* outfile_verif_results = nullptr;
bool outfile_verif_results_has_been_opened = false;

void DisplayVerify :: execute(imgdata_t *imdata, std::string args){
    cout << "DisplayVerify , ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;
	
    bool both_cseg_and_sseg_succeeded = (imdata->sseg_image_data->empty()==false && imdata->sseg_image_data->empty()==false);
	bool save_output_file = false;

    if(outfile_verif_results == nullptr) {
		if(outfile_verif_results_has_been_opened == false) {
			outfile_verif_results = new std::fstream();
			std::string output_folder("../../output_images");
			save_output_file = (check_if_directory_exists(output_folder) && outfile_verif_results!=nullptr);
			if(save_output_file) {
				outfile_verif_results_has_been_opened = true;
				outfile_verif_results->open(output_folder+std::string("/verif_results.txt"),
						std::fstream::trunc | std::fstream::out); //clear the file of data (if it existed): we're about to rewrite it
				save_output_file = (outfile_verif_results->is_open() && outfile_verif_results->good());
			}
		}
	}
	else {
		save_output_file = (outfile_verif_results->is_open() && outfile_verif_results->good());
	}

    if(imdata->shape.empty())
        cout << "no shape found!" << endl;
    else
        cout << "shape found: \'" << imdata->shape << "\'" << endl;

    if(imdata->character.empty())
        cout << "no character found!" << endl;
    else
        cout << "character found: \'" << imdata->character << "\'" << endl;

    if(both_cseg_and_sseg_succeeded && save_output_file) {
		cout << "==============================wrote results to file" << endl;
		(*outfile_verif_results) << "--------------------------------" << endl;
		(*outfile_verif_results) << "target #" << imdata->id << endl;
		(*outfile_verif_results) << "shape: " << imdata->shape << endl;
		(*outfile_verif_results) << "char:  " << imdata->character << endl;
		(*outfile_verif_results) << std::flush;
	}

    if(kShowImages)
    {
        cv::startWindowThread();
        cv::namedWindow("Image", CV_WINDOW_NORMAL);

        for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->image_data->begin();
                i < imdata->image_data->end(); ++i){
            cv::imshow("Image",	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
            cv::waitKey(0);
        }

        if(!kShowIfSsegCsegSuccess || (!imdata->sseg_image_data->empty() && !imdata->sseg_image_data->empty()))
        {
            for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
                    i < imdata->sseg_image_data->end(); ++i){
                cv::imshow("Image",	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
                cv::waitKey(0);
            }

            for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
                    i < imdata->cseg_image_data->end(); ++i){
                cv::imshow("Image",	cv::imdecode(**i, CV_LOAD_IMAGE_COLOR));
                cv::waitKey(0);
            }
        }
        cv::destroyWindow("Image");
    }

    setDone(imdata, VERIF);
}
