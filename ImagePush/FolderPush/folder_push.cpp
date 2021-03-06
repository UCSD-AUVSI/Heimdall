#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <thread>
using std::cout;
using std::endl;

#include "ImagePush/FolderPush/folder_push.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

#include "opencv2/opencv.hpp"

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "SharedUtils/exif.h"



const bool kUseEXIFForInfo = true;
double GroundLevelAltitude_in_m = 0.0;



const bool DEBUG_EXIF_INFO = true;

int FolderPush::sendcount = 0, FolderPush::delay = 100;

bool FolderPush::send = true, FolderPush::pause = false,
     FolderPush::search_subfolders = false, FolderPush::first_send = true;

std::vector<std::string> * FolderPush::file_list = new std::vector<std::string>();
std::vector<std::string> * FolderPush::file_list_backup = new std::vector<std::string>();


void FolderPush :: usage(){
	cout << "Usage: --images FOLDER_PUSH [OPTIONS]..."  << endl;
	cout << "Parses a directory for image files, and pushes one at a regular interval" << endl;

	cout << "Command Line Options: \n" << endl;
	cout << "   --folder       Path to folder with test images\n" << endl;
	cout << "   --subfolders   When used with --folder, will search subfolders of --folder too." << endl;
}

void FolderPush :: processArguments(std::string args, std::string& folder){
    std::vector<std::string> arglist = split(args, ' ');

    for(int i = 0; i < arglist.size(); i++){
        std::string arg = arglist[i]; 
        if(arg == "--folder"){
            if(++i >= arglist.size()){
                cout << "FolderPush argument list incorrectly formatted" << endl;
                return;
            }
            folder = arglist[i];
        }
        else if (arg == "--subfolders"){
            FolderPush :: search_subfolders = true;
        }
        else{
            FolderPush::usage();
            FolderPush::send = false;
            return;
        }
    }
}

int FolderPush :: FindAllImagesInDir(std::string dirpath, int subdir_recursion_depth_limit) {
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
			if(filename_extension_is_image_type(get_extension_from_filename(std::string(file.name)))) {
                num_files_found++;
                FolderPush::file_list -> push_back(std::string(file.path));
                FolderPush::file_list_backup->push_back(std::string(file.path));
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
    return num_files_found;
}

void FolderPush :: execute(imgdata_t *imdata, std::string args){
    //Return immediately if not sending
    if(!FolderPush::send){
		//cout<<"FOLDERPUSH: !SEND"<<endl;
        std::chrono::milliseconds dura(FolderPush::delay);
        std::this_thread::sleep_for(dura);
        return;
    }
	//cout<<"FOLDERPUSH: SENDING SOMETHING"<<endl;
    
    //Pause before continuing, to prevent pushing too fast
    if(FolderPush::pause){
        std::chrono::milliseconds dura(FolderPush::delay);
        std::this_thread::sleep_for(dura);
    }

    // If we have not sent yet, initialize various elements
    // and populate file list from folder
    if(FolderPush::first_send){
        std::string folder = "./push_files";
        FolderPush::processArguments(args, folder);
        cout << "Found " << FindAllImagesInDir(folder, FolderPush::search_subfolders?2:0)
            << " files in folder " << folder << endl;

        FolderPush::pause = true;
        FolderPush::first_send = false;
        
        globalNumImagesInExperiment = FolderPush::file_list->size();
    }
    
    //If we are finished with all files, we are done sending
    if(FolderPush::file_list -> size() <= 0){
        FolderPush::send = false;
        cout << "Done with all files in folder" << endl;

        delete FolderPush::file_list;
        return;
    }

    //Get next file
    std::string image = FolderPush::file_list ->back();
    FolderPush::file_list -> pop_back();

    cout << "Running with following parameters: \n" << endl;
    cout << "Image: " << image << endl;
    
    //get image filename for debugging
    std::string justTheImageName(replace_char_in_string(image,'\\','/'));
    justTheImageName = trim_chars_after_delim(justTheImageName, '/', false);
    imdata->name_of_original_image_file_for_debugging = justTheImageName;

    //Check if image exists
    if(FILE *file = fopen(image.c_str(), "rb")){
        if(kUseEXIFForInfo){
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
			GroundLevelAltitude_in_m = file_exif.GeoLocation.GPSSpeed;
			
            imdata->planelat      = file_exif.GeoLocation.Latitude;
            imdata->planelongt    = file_exif.GeoLocation.Longitude;
            imdata->planealt      = (file_exif.GeoLocation.Altitude - GroundLevelAltitude_in_m) * 3.28084; //meters -> feet
            imdata->planeheading  = file_exif.GeoLocation.ImgDirection;
			
            if(DEBUG_EXIF_INFO) {
				consoleOutput.Level4()<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
				consoleOutput.Level0()<<"image loaded: imdata->plane: (lat,longt) and altitude and heading == ("<<imdata->planelat<<", "<<imdata->planelongt<<") and "<<(imdata->planealt)<<" feet at heading "<<(imdata->planeheading)<<endl;
			}
        }
        else{
            fclose(file);
        }
    }
    else{
        cout << "FolderPush: Image not found!" << endl;
        return;
    }


    // Increment sendcount, each image has an individual id
    imdata->id = FolderPush::sendcount ++;
    cout << "Reading " << endl;
    // Load image into struct
    std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
    cv::imencode(".jpg", cv::imread(image, CV_LOAD_IMAGE_COLOR), *newarr);
    imdata->image_data = newarr;

    cout << "Sending " << messageSizeNeeded(imdata) << " bytes. File ID: " << imdata->id << endl << endl;
}
