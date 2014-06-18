#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

using std::cout;
using std::endl;

#include "ImagePush/FolderWatch/folder_watch.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

#include "opencv2/opencv.hpp"

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "SharedUtils/exif.h"

int FolderWatch::sendcount = 0, FolderWatch::delay = 500;

bool FolderWatch::send = true, FolderWatch::pause = false,
     FolderWatch::search_subfolders = false, FolderWatch::first_send = true;

bool kOnGround = false;

std::vector<std::pair<std::string, std::string>> * FolderWatch::file_list = new std::vector<std::pair<std::string, std::string>>();
std::map<std::string, std::pair<std::string, std::string>> * file_map = new std::map<std::string, std::pair<std::string, std::string>>();
std::set<std::string> * seen_image_set = new std::set<std::string>();
std::set<std::string> * seen_info_set  = new std::set<std::string>();

std::string folder = "./push_files"; //Changed via arguments

int unpaired_files = 0;
int refresh_count = 0;
const int kRefreshRounds = 5;

void FolderWatch :: usage(){
    cout << "Usage: --images FOLDER_WATCH [OPTIONS]..."  << endl;
    cout << "Watches a directory for image files and their matching information files, and pushes one at a regular interval" << endl;

    cout << "Command Line Options: \n" << endl;
    cout << "   --folder       Path to folder with test images\n" << endl;
    cout << "   --subfolders   When used with --folder, will search subfolders of --folder too." << endl;
}

void FolderWatch :: processArguments(std::string args, std::string& folder){
    std::vector<std::string> arglist = split(args, ' ');

    for(int i = 0; i < arglist.size(); i++){
        std::string arg = arglist[i]; 
        if(arg == "--folder"){
            if(++i >= arglist.size()){
                cout << "FolderWatch argument list incorrectly formatted" << endl;
                return;
            }
            folder = arglist[i];
        }
        else if (arg == "--subfolders"){
            FolderWatch :: search_subfolders = true;
        }
        else{
            FolderWatch::usage();
            FolderWatch::send = false;
            return;
        }
    }
}

int FolderWatch :: FindAllImagesInDir(std::string dirpath, int subdir_recursion_depth_limit) {
    int num_files_found=0;
    tinydir_dir dir;
    tinydir_open(&dir, dirpath.c_str());
    while(dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        if(file.is_dir && subdir_recursion_depth_limit > 0 && file.name[0] != '.') {
            num_files_found += FindAllImagesInDir(file.path, subdir_recursion_depth_limit - 1);
        }
        else {
            std::string filename(file.name);
            eliminate_extension_from_filename(filename);

            if(filename_extension_is_image_type(get_extension_from_filename(std::string(file.name)))) {
                if(seen_image_set->find(filename) == seen_image_set->end()){
                    num_files_found++;
                    seen_image_set->insert(filename);

                    try{
                        std::pair<std::string, std::string> file_pair = file_map->at(filename);
                        std::get<0>(file_pair) = std::string(file.path);

                        FolderWatch::file_list->push_back(file_pair);

                        unpaired_files--;
                    }
                    catch(std::out_of_range &oor){
                        std::pair<std::string, std::string> new_file_pair(std::string(file.path), "");
                        (*file_map)[filename] = new_file_pair;
                        unpaired_files++;
                    }
                }
            }
            else if(get_extension_from_filename(std::string(file.name)) == ".txt"){
                if(seen_info_set->find(filename) == seen_info_set->end()){
                    seen_info_set->insert(filename);

                    try{
                        std::pair<std::string, std::string> file_pair = file_map->at(filename);
                        std::get<1>(file_pair) = std::string(file.path);

                        FolderWatch::file_list->push_back(file_pair);

                        unpaired_files--;
                    }
                    catch(std::out_of_range &oor){
                        std::pair<std::string, std::string> new_file_pair("", std::string(file.path));
                        (*file_map)[filename] = new_file_pair;
                        unpaired_files++;
                    }
                }
            }
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);
    return num_files_found;
}

void FolderWatch :: execute(imgdata_t *imdata, std::string args){
    //Return immediately if not sending
    if(!FolderWatch::send){
        std::chrono::milliseconds dura(FolderWatch::delay);
        std::this_thread::sleep_for(dura);
        return;
    }

    //Pause before continuing, to prevent pushing too fast
    if(FolderWatch::pause){
        std::chrono::milliseconds dura(FolderWatch::delay);
        std::this_thread::sleep_for(dura);
    }

    // If we have not sent yet, initialize various elements
    // and populate file list from folder
    if(FolderWatch::first_send){
        FolderWatch::processArguments(args, folder);

        FolderWatch::pause = true;
        FolderWatch::first_send = false;
    }

    if((refresh_count++) % kRefreshRounds == 0){
        cout << "Found " << FolderWatch::FindAllImagesInDir(folder, FolderWatch::search_subfolders?2:0)
            << " files in folder " << folder << endl;
        cout << "Unpaired files: " << unpaired_files << endl << endl;
    }

    if(FolderWatch::file_list->size() == 0) { //No files, just return
        return;
    }

    //Get next file
    std::pair<std::string, std::string> file_pair = FolderWatch::file_list->back();
    FolderWatch::file_list -> pop_back();

    std::string image = std::get<0>(file_pair);
    std::string info  = std::get<1>(file_pair);
    std::string infoline;

    cout << "Running with following parameters: \n" << endl;
    cout << "Image: " << image << endl;

    //Check if image exists
    if(FILE *file = fopen(image.c_str(), "rb")){
        fclose(file);
    }
    else{
        cout << "FolderWatch: Image not found!" << endl;
        return;
    }

    if(FILE *file = fopen(info.c_str(), "rb")){
        fclose(file);

        std::ifstream ifs;
        ifs.open(info);

        std::getline(ifs, infoline);

        ifs.close();
    }
    else{
        cout << "FolderWatch: Info file not found!" << endl;
        return;
    }

    // Increment sendcount, each image has an individual id
    imdata->id = FolderWatch::sendcount ++;

    // Load image into struct
    std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
    cv::imencode(".jpg", cv::imread(image, CV_LOAD_IMAGE_COLOR), *newarr);
    imdata->image_data = newarr;

    std::vector<std::string> split_line = split(infoline, '\t');

    if (kOnGround) {
        if(split_line.size() != 9){
            cout << "Crash imminent, input info file from plane should have 9 attributes." << endl;
        }

        imdata->planeroll =         atof(split_line.at(0).c_str());
        imdata->planepitch =        atof(split_line.at(1).c_str());
        imdata->planeheading =      atof(split_line.at(2).c_str());
        imdata->planelat =          atof(split_line.at(3).c_str());
        imdata->planelongt =        atof(split_line.at(4).c_str());
        imdata->planealt =          atof(split_line.at(5).c_str()); //Using AGL, not MSL
        imdata->targetlat =         atof(split_line.at(6).c_str());
        imdata->targetlongt =       atof(split_line.at(7).c_str());
        imdata->targetorientation = atof(split_line.at(8).c_str());
    } else {
        if(split_line.size() < 8){
            cout << "Crash imminent, input info file has less than 8 attributes. Nathan has not adhered to his end of the contract." << endl;
        }

        imdata->planeroll =     atof(split_line.at(1).c_str());
        imdata->planepitch =    atof(split_line.at(2).c_str());
        imdata->planeheading =  atof(split_line.at(3).c_str());
        imdata->planelat =      atof(split_line.at(4).c_str());
        imdata->planelongt =    atof(split_line.at(5).c_str());
        imdata->planealt =      atof(split_line.at(7).c_str()); //Using AGL, not MSL
    }

    cout << "Sending " << messageSizeNeeded(imdata) << " bytes. File ID: " << imdata->id << endl << endl;
}
