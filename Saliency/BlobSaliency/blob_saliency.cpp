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


Blob_Saliency_Module* global_sal_instance = nullptr; //declared in BlobSaliency.hpp

using std::cout;
using std::endl;

void BlobSaliency :: execute(imgdata_t *imdata, std::string args){
    cout << "BlobSaliency, ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;

    if(global_sal_instance == nullptr) {
        global_sal_instance = new Blob_Saliency_Module();
        
        global_sal_instance->write_internal_images = false;
        global_sal_instance->output_dir = std::string("./debug/Saliency");
        bool environment_is_desert = false;

        if(environment_is_desert) {
            Blob_Saliency_Settings desert_settings;
            desert_settings.canny_low_thresh[0] = 50;
            desert_settings.canny_low_thresh[1] = 20;
            desert_settings.canny_low_thresh[2] = 20;
            desert_settings.canny_high_thresh[0] = 140;
            desert_settings.canny_high_thresh[1] = 60;
            desert_settings.canny_high_thresh[2] = 60;
            desert_settings.max_ellipse_area_base = 100.0f;
            desert_settings.min_ellipse_area_base = 0.2f;
            desert_settings.converted_color_space = CV_BGR2Lab;
            desert_settings.blur_kernel_size[0] = 5;
            desert_settings.blur_kernel_size[1] = 5;
            desert_settings.blur_kernel_size[2] = 5;
            global_sal_instance->settings.push_back(desert_settings);
        } else {
            global_sal_instance->settings.push_back(Blob_Saliency_Settings());
        }

        if(global_sal_instance->write_internal_images){
            if(!check_if_directory_exists(global_sal_instance->output_dir)){
#ifdef WIN32
                if(!_mkdir(global_sal_instance->output_dir.c_str())){
                    cout << "Could not create output directory, not writing images" << endl;
                    global_sal_instance -> write_internal_images = false;
                }
#else
                if(!mkdir(global_sal_instance->output_dir.c_str(), S_IRWXU | S_IRWXG)){
                    cout << "Could not create output directory, not writing images" << endl;
                    global_sal_instance -> write_internal_images = false;
                }
#endif
            }
        }
    }


    global_sal_instance->do_saliency(cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR), imdata);
    imdata->image_data->clear();

    //after saliency is done with the fullsize image, remove that fullsize image from the message's images vector

    consoleOutput.Level1() << "Saliency found "
        << to_istring(global_sal_instance->return_crops.size())
        << " crops" << std::endl;

    std::vector<int> param = std::vector<int>(2);
    param[0] = CV_IMWRITE_PNG_COMPRESSION;
    param[1] = 3; //default(3)  0-9, where 9 is smallest compressed size.

    imgdata_t *curr_imdata = imdata;
    int i = 0;
    while(i < global_sal_instance->return_crops.size()){
        std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
        cv::imencode(".png", global_sal_instance->return_crops.at(i) , *newarr, param);
        delete curr_imdata->image_data; //delete whatever is already in there
        curr_imdata->image_data = newarr;

        std::pair<double,double> target_geoloc = global_sal_instance->return_geolocs.at(i);
        curr_imdata->targetlat = std::get<0>(target_geoloc);
        curr_imdata->targetlongt = std::get<1>(target_geoloc);

        if(++i < global_sal_instance->return_crops.size()){
            imgdata_t *new_imdata = new imgdata_t();
            copyIMGData(new_imdata, curr_imdata);

            new_imdata->next = nullptr;
            new_imdata->cropid++;
            curr_imdata->next = new_imdata;
            curr_imdata = new_imdata;
        }
    }

    setDone(imdata, SALIENCY);
}
