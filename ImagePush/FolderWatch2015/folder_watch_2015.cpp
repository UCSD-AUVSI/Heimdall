#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

using std::cout;
using std::endl;

#include "ImagePush/FolderWatch2015/folder_watch_2015.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

#include "opencv2/opencv.hpp"

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "SharedUtils/exif.h"

int FolderWatch2015::sendcount = 0, FolderWatch2015::delay = 500;

bool FolderWatch2015::send = true, FolderWatch2015::pause = false,
     FolderWatch2015::search_subfolders = false, FolderWatch2015::first_send = true;

std::vector<std::string> * FolderWatch2015::file_list = new std::vector<std::string>();
std::set<std::string> * FolderWatch2015::seen_image_set = new std::set<std::string>();

std::string * FolderWatch2015::watchfolder = new std::string("."); //Changed via arguments

int FolderWatch2015::refresh_count = 0;
const int kRefreshRounds = 5;

void FolderWatch2015 :: usage(){
    cout << "Usage: --images FOLDER_WATCH [OPTIONS]..."  << endl;
    cout << "Watches a directory for image files and their matching information files, and pushes one at a regular interval" << endl;

    cout << "Command Line Options: \n" << endl;
    cout << "   --folder       Path to folder with test images\n" << endl;
    cout << "   --subfolders   When used with --folder, will search subfolders of --folder too." << endl;
}

void FolderWatch2015 :: processArguments(std::string args, std::string& folder){
    std::vector<std::string> arglist = split(args, ' ');

    for(int i = 0; i < arglist.size(); i++){
        std::string arg = arglist[i]; 
        if(arg == "--folder"){
            if(++i >= arglist.size()){
                cout << "FolderWatch2015 argument list incorrectly formatted" << endl;
                return;
            }
            folder = arglist[i];
        }
        else if (arg == "--subfolders"){
            FolderWatch2015 :: search_subfolders = true;
        }
        else{
            FolderWatch2015::usage();
            FolderWatch2015::send = false;
            return;
        }
    }
}

int FolderWatch2015 :: FindAllImagesInDir(std::string dirpath, int subdir_recursion_depth_limit) {
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
                if(seen_image_set->find(filename) == seen_image_set->end()) {
                    num_files_found++;
                    seen_image_set->insert(filename);
                    FolderWatch2015::file_list->push_back(file.path);
				}
            }
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);
    return num_files_found;
}

void FolderWatch2015 :: execute(imgdata_t *imdata, std::string args){
    //Return immediately if not sending
    if(!FolderWatch2015::send){
        std::chrono::milliseconds dura(FolderWatch2015::delay);
        std::this_thread::sleep_for(dura);
        return;
    }

    //Pause before continuing, to prevent pushing too fast
    if(FolderWatch2015::pause){
        std::chrono::milliseconds dura(FolderWatch2015::delay);
        std::this_thread::sleep_for(dura);
    }

    // If we have not sent yet, initialize various elements
    // and populate file list from folder
    if(FolderWatch2015::first_send){
        FolderWatch2015::processArguments(args, *watchfolder);

        FolderWatch2015::pause = true;
        FolderWatch2015::first_send = false;
    }

    if((refresh_count++) % kRefreshRounds == 0){
        cout << "Found " << FolderWatch2015::FindAllImagesInDir(*watchfolder, FolderWatch2015::search_subfolders?2:0)
            << " files in folder " << (*watchfolder) << endl;
    }

    if(FolderWatch2015::file_list->size() == 0) { //No files, just return
        return;
    }

    //Get next file
    std::string image_filename = FolderWatch2015::file_list->back();
    FolderWatch2015::file_list -> pop_back();

    cout << "Running with following parameters: \n" << endl;
    cout << "Image: " << image_filename << endl;

    //Check if image exists
    if(FILE *file = fopen(image_filename.c_str(), "rb")){
        fclose(file);
    }
    else{
        cout << "FolderWatch2015: Image not found!" << endl;
        return;
    }

    // Increment sendcount, each image has an individual id
    imdata->id = FolderWatch2015::sendcount ++;

    // Load image into struct
    std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
    cv::imencode(".jpg", cv::imread(image_filename, CV_LOAD_IMAGE_COLOR), *newarr);
    imdata->image_data = newarr;

    
//  THE FOLLOWING LINES SHOULD BE THE SAME AS IN FOLDER_PUSH
    
    //get image filename for debugging
    std::string justTheImageName(replace_char_in_string(image_filename,'\\','/'));
    justTheImageName = trim_chars_after_delim(justTheImageName, '/', false);
    imdata->name_of_original_image_file_for_debugging = justTheImageName;

    //Check if image exists
    if(FILE *file = fopen(image_filename.c_str(), "rb")){
            cout << "Using EXIF for info" << endl;

            fseek(file, 0, SEEK_END);
            unsigned long fsize = ftell(file);
            rewind(file);
            
            unsigned char *buf = new unsigned char[fsize];
            if(fread(buf,1,fsize,file) != fsize){
                cout << "Can't read EXIF, ignoring" << endl;
                delete[] buf;
            }
            fclose(file);

            EXIFInfo file_exif;
            int code = file_exif.parseFrom(buf, fsize);
            delete[] buf;

            if(code){
                cout << "Error parsing EXIF code" << endl;
            }
			
			// this is hacked in because EXIF does not normally have a field for the altitude of the ground
			// when we record "planealt" we want to record RELATIVE TO THE GROUND
			double GroundLevelAltitude_in_m = file_exif.GeoLocation.GPSSpeed;
			
            imdata->planelat      = file_exif.GeoLocation.Latitude;
            imdata->planelongt    = file_exif.GeoLocation.Longitude;
            imdata->planealt      = (file_exif.GeoLocation.Altitude - GroundLevelAltitude_in_m) * 3.28084; //meters -> feet
            imdata->planeheading  = file_exif.GeoLocation.ImgDirection;
			
            if(true) {
				consoleOutput.Level4()<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
				consoleOutput.Level0()<<"image loaded: imdata->plane: (lat,longt) and altitude and heading == ("<<imdata->planelat<<", "<<imdata->planelongt<<") and "<<(imdata->planealt)<<" feet at heading "<<(imdata->planeheading)<<endl;
			}
    }
    else{
        cout << "FolderWatch2015: Image not found!" << endl;
        return;
    }
    
//  THE PREVIOUS LINES SHOULD BE THE SAME AS IN FOLDER_PUSH
    

    cout << "Sending " << messageSizeNeeded(imdata) << " bytes. File ID: " << imdata->id << endl << endl;
}
