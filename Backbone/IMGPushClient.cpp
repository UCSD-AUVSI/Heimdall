#include <string>
#include <map>
#include <vector>

#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/MessageHandling.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/IMGPushClient.hpp"

#include "opencv2/opencv.hpp"

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"

using std::cout;
using std::endl;

/***
 * IMGPushClient
 * Pushes images to a designated port
 */

IMGPushClient :: IMGPushClient(std::string addr, std::string imageloc){
	server_addr = addr;
	image = imageloc;
}

void IMGPushClient :: run(){
	std::thread workThr(&IMGPushClient::work, this);
	workThr.detach();	
}

void IMGPushClient :: work(){
	zmq::context_t context(1);

	zmq::socket_t pushsocket(context, ZMQ_PUSH);
	std::string addr = "tcp://" + server_addr + ":" + std::to_string(IMAGES_PULL);
	pushsocket.connect(addr.c_str());

	int sendcount = 0;
	while(sendcount == 0){
		imgdata_t imdata;
		imdata.id = sendcount++;
		initEmptyIMGData(&imdata);
		
		std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
		cv::imencode(".jpg", cv::imread(image, CV_LOAD_IMAGE_COLOR), *newarr);
		imdata.image_data->push_back(newarr);
		
		/*
		newarr = new std::vector<unsigned char>();
		cv::imencode(".jpg", cv::imread(image, CV_LOAD_IMAGE_GRAYSCALE), *newarr);
		imdata.sseg_image_data->push_back(newarr);
		
		newarr = new std::vector<unsigned char>();
		cv::imencode(".jpg", cv::imread(image), *newarr);
		imdata.cseg_image_data->push_back(newarr);
		*/

		zmq::message_t msg(messageSizeNeeded(&imdata));
		packMessageData(&msg, &imdata);

		cout << "Sending " << msg.size() << " bytes" << endl;

		pushsocket.send(msg);
		
		clearIMGData(&imdata);
		
		std::chrono::milliseconds dura(5000);
		std::this_thread::sleep_for(dura);
	}
	
	//this detached thread will close now
	//this should be the last line of code this class ever runs
	//see run() - after detaching, this class isn't referred to anymore
	delete this;
}

void IMGPushClient :: usage(){
	cout << "Usage: ./IMGPushClient [OPTION]..."  << endl;
	cout << "Starts a client that pushes images to the server every 1 second\n" << endl;

	cout << "Command Line Options: \n" << endl;
	cout << "   --server       Server IP Address (default localhost)\n" << endl;
	cout << "   --image        Path to image to push (default ./foo.jpg)\n" << endl;
	cout << "   --folder       Path to folder with test images (overrides option --image)\n" << endl;
	cout << "   --subfolders   When used before --folder, will search subfolders of --folder too." << endl;
}

int main(int argc, char* argv[]){

	std::string addr = "localhost"; //Default Server Address
	std::string image = "./foo.jpg"; //Default Image Location
	std::string folder_path_str("");
	bool search_subfolders_too = false;

	//Read in command line arguments
	for(int i = 1; i < argc; i++){
		std::string arg = std::string(argv[i]);	
		if(arg == "--server"){
			if(++i >= argc){
				IMGPushClient::usage();
				return -1;
			}
			addr = std::string(arg);
		}
		else if(arg == "--subfolders"){
			search_subfolders_too = true;
		}
		else if(arg == "--folder"){
			if(++i >= argc){
				IMGPushClient::usage();
				return -1;
			}
			folder_path_str = std::string(argv[i]);
		}
		else if(arg == "--image"){
			if(++i >= argc){
				IMGPushClient::usage();
				return -1;
			}
			image = std::string(argv[i]);
		}
		else{
			IMGPushClient::usage();
			return -1;
		}
	}
	
	cout << "Running with following parameters: \n" << endl;
	cout << "Server Address: " << addr << endl;

	if(folder_path_str.empty()) {
		CreatePushClientForImage(addr, image);
	}
	else {
		cout << "Folder: " << folder_path_str << endl;
		if(search_subfolders_too) {
			cout << "Searching subdirectories, recursion depth limit is 2." << endl;
		}
		OpenAndPushAllImagesInDir(folder_path_str, search_subfolders_too ? 2 : 0, addr);
	}

	cout << "Press any key to exit. \n\n" << endl;
	getchar();

	return 0;
}

void OpenAndPushAllImagesInDir(std::string dirpath, int subdir_recursion_depth_limit, std::string& server_address) {
	int num_files_found=0;
	tinydir_dir dir;
	tinydir_open(&dir, dirpath.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir && subdir_recursion_depth_limit > 0 && file.name[0] != '.') {
			OpenAndPushAllImagesInDir(file.path, subdir_recursion_depth_limit - 1, server_address);
		}
		else {
			num_files_found++;
			if(filename_extension_is_image_type(get_extension_from_filename(std::string(file.name)))) {
				CreatePushClientForImage(server_address, std::string(file.path));
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
}

void CreatePushClientForImage(std::string server_address, std::string image_filename){
	//Check if image exists
	if(check_if_file_exists(image_filename) == false){
		cout << "failed to find or open image \"" << image_filename << "\"" << endl;
		return;
	}

	cout << "Running with image: " << image_filename << endl;

	IMGPushClient* ipc = new IMGPushClient(server_address, image_filename);
	ipc->run();
	
	//will delete itself after the thread finishes
}
