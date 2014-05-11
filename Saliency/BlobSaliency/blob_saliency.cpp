#include <string>
#include <vector>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"

#include "blob_saliency.hpp"
#include "blob_saliency_module.hpp"

// For mkdir or _mkdir
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif


Blob_Saliency_Module* global_blobsaliency_instance = nullptr; //declared in BlobSaliency.hpp

using std::cout;
using std::endl;

void BlobSaliency :: execute(imgdata_t *imdata, std::string args){
    cout << "BlobSaliency, ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;

    if(global_blobsaliency_instance == nullptr) {
        global_blobsaliency_instance = new Blob_Saliency_Module();
        
        global_blobsaliency_instance->write_internal_images = false;
        global_blobsaliency_instance->output_dir = std::string("./debug/Saliency");
        bool environment_is_desert = false;

        if(environment_is_desert) {
            Blob_Saliency_Settings desert_settings;
            desert_settings.canny_low_thresh[0] = 35;
            desert_settings.canny_low_thresh[1] = 25;
            desert_settings.canny_low_thresh[2] = 25;
            desert_settings.canny_high_thresh[0] = 70;
            desert_settings.canny_high_thresh[1] = 50;
            desert_settings.canny_high_thresh[2] = 50;
            desert_settings.max_ellipse_area_base = 80.0f;
            desert_settings.min_ellipse_area_base = 5.0f;
            desert_settings.converted_color_space = CV_BGR2Lab;
            global_blobsaliency_instance->settings.push_back(desert_settings);
        } else {
            global_blobsaliency_instance->settings.push_back(Blob_Saliency_Settings());
            global_blobsaliency_instance->settings.rbegin()->keep_color_channel[0] = false;
        }

        if(global_blobsaliency_instance->write_internal_images){
            if(!check_if_directory_exists(global_blobsaliency_instance->output_dir)){
#ifdef WIN32
                if(!_mkdir(global_blobsaliency_instance->output_dir.c_str())){
                    cout << "Could not create output directory, not writing images" << endl;
                    global_blobsaliency_instance -> write_internal_images = false;
                }
#else
                if(!mkdir(global_blobsaliency_instance->output_dir.c_str(), S_IRWXU | S_IRWXG)){
                    cout << "Could not create output directory, not writing images" << endl;
                    global_blobsaliency_instance -> write_internal_images = false;
                }
#endif
            }
        }
    }


    std::vector<std::vector<unsigned char>*>::iterator i = imdata->image_data->begin();
    while(i != imdata->image_data->end()) {
        global_blobsaliency_instance->do_saliency(cv::imdecode(**i, CV_LOAD_IMAGE_COLOR), imdata);

        //after saliency is done with the fullsize image, remove that fullsize image from the message's images vector
        (*i)->clear();
        delete (*i);
        i = imdata->image_data->erase(i);
    }

    if(imdata->image_data->empty()==false) {
        cout << "WARNING: message struct's vector of images wasn't fully cleared before saliency started pushing back crops!" << std::endl;
    }

    consoleOutput.Level1() << "Saliency found "
        << to_istring(global_blobsaliency_instance->returned_cropped_images.size())
        << " crops" << std::endl;

    std::vector<int> param = std::vector<int>(2);
    param[0] = CV_IMWRITE_PNG_COMPRESSION;
    param[1] = 3; //default(3)  0-9, where 9 is smallest compressed size.
    for(std::vector<cv::Mat>::iterator criter = global_blobsaliency_instance->returned_cropped_images.begin();
            criter != global_blobsaliency_instance->returned_cropped_images.end(); criter++) {
        std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
        cv::imencode(".png", *criter, *newarr, param);
        imdata->image_data->push_back(newarr);
    }

    setDone(imdata, SALIENCY);
}
