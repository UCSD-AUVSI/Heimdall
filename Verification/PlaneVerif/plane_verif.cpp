#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "Verification/PlaneVerif/plane_verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "opencv2/opencv.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp" //for saveImage

using std::cout;
using std::endl;

const bool kSaveImages = true;

void PlaneVerify :: usage(){
    cout << "Usage: --verif PLANE_VERIF [OPTIONS]..."  << endl;
    cout << "Saves images and info files to output folder" << endl;

    cout << "Command Line Options: \n" << endl;
    cout << "   --folder       Path to output folder\n" << endl;
}

void PlaneVerify :: processArguments(std::string args, std::string& folder){
    std::vector<std::string> arglist = split(args, ' ');

    for(int i = 0; i < arglist.size(); i++){
        std::string arg = arglist[i]; 
        if(arg == "--folder"){
            if(++i >= arglist.size()){
                cout << "PlaneVerify argument list incorrectly formatted" << endl;
                PlaneVerify::usage();
                return;
            }
            folder = arglist[i];
        }
        else{
            PlaneVerify::usage();
            return;
        }
    }
}

void PlaneVerify :: execute(imgdata_t *imdata, std::string args){
    cout << "PlaneVerify , ID: " << imdata->id  << ", CropID: " << imdata->cropid << endl;

    std::string output_folder("../../output_images");
    PlaneVerify::processArguments(args, output_folder);

    // Save images and crops
    std::string name_of_input_crop = std::to_string(imdata->id) + "_" + std::to_string(imdata->cropid);
    if(kSaveImages && check_if_directory_exists(output_folder)) {
        if(imdata->image_data->size()){
            cv::Mat image = cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR);
            saveImage(image, output_folder + "/" + name_of_input_crop + ".jpg");

            std::ofstream ofs;
            ofs.open(output_folder + "/" + name_of_input_crop + ".txt");
            
            ofs << std::setprecision(10) << imdata->planelat            << "\t";
            ofs << std::setprecision(10) << imdata->planelongt          << "\t";
            ofs << std::setprecision(10) << imdata->planealt            << "\t";
            ofs << std::setprecision(10) << imdata->planeroll           << "\t";
            ofs << std::setprecision(10) << imdata->planepitch          << "\t";
            ofs << std::setprecision(10) << imdata->planeheading        << "\t";
            ofs << std::setprecision(10) << imdata->targetlat           << "\t";
            ofs << std::setprecision(10) << imdata->targetlongt         << "\t";
            ofs << std::setprecision(10) << imdata->targetorientation   << "\t";

            ofs.close();
        }
    
    }
    setDone(imdata, VERIF);
}
