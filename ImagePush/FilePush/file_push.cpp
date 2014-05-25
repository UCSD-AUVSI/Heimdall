#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <thread>

using std::cout;
using std::endl;

#include "ImagePush/FilePush/file_push.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

#include "opencv2/opencv.hpp"

int FilePush::sendcount = 0;

bool FilePush::send = true, FilePush::pause = false;

std::vector<std::string>& FilePush :: split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> FilePush :: split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    FilePush::split(s, delim, elems);
    return elems;
}

void FilePush :: processArguments(std::string args, std::string& image){
    std::vector<std::string> arglist = FilePush::split(args, ' ');

    for(int i = 0; i < arglist.size(); i++){
        std::string arg = arglist[i]; 
        if(arg == "--image"){
            if(++i >= arglist.size()){
                cout << "FilePush argument list incorrectly formatted" << endl;
                return;
            }
            image = arglist[i];
        }
        else{
            cout << "FilePush argument list incorrectly formatted" << endl;
            return;
        }
    }
}

void FilePush :: execute(imgdata_t *imdata, std::string args){
    if(!FilePush::send){
        std::chrono::milliseconds dura(5000);
        std::this_thread::sleep_for(dura);
        return;
    }

    if(FilePush::pause){
        std::chrono::milliseconds dura(5000);
        std::this_thread::sleep_for(dura);
    }

    std::string image = "./foo.jpg";

    FilePush::processArguments(args, image);

    //Check if image exists
    if(FILE *file = fopen(image.c_str(), "r")){
        fclose(file);
    }
    else{
        cout << "FilePush: Image not found!" << endl;
        FilePush::pause = true;
        return;
    }

    cout << "Running with following parameters: \n" << endl;
    cout << "Image: " << image << endl;

    imdata->id = FilePush::sendcount ++;

    std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
    cv::imencode(".jpg", cv::imread(image, CV_LOAD_IMAGE_COLOR), *newarr);
    imdata->image_data = newarr;

    cout << "Sending " << messageSizeNeeded(imdata) << " bytes. File ID: " << imdata->id << endl << endl;

    FilePush::send = false;
    FilePush::pause = true;
}
