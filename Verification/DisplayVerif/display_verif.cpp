#include <string>
#include <iostream>

#include "Verification/DisplayVerif/display_verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;

const bool kShowImages = true;
const bool kShowIfSsegCsegSuccess = true;

void DisplayVerify :: execute(imgdata_t *imdata, std::string args){
    cout << "DisplayVerify , ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;

    if(imdata->shape.empty())
        cout << "no shape found!" << endl;
    else
        cout << "shape found: \'" << imdata->shape << "\'" << endl;

    if(imdata->character.empty())
        cout << "no character found!" << endl;
    else
        cout << "character found: \'" << imdata->character << "\'" << endl;

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
