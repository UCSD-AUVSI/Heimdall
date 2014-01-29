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

	int count = 0;
	while(true){
		imgdata_t data;
		data.id = count++;
		
		data.image_data = new std::vector<unsigned char>();			
		cv::imencode(".jpg", cv::imread(image), *(data.image_data));
		
		zmq::message_t msg(messageSizeNeeded(&data));
		packMessageData(&msg, &data);

		cout << "Sending " << msg.size() << " bytes" << endl;

		pushsocket.send(msg);
		
		freeIMGData(&data);
		
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);
	}
}

void IMGPushClient :: usage(){
	cout << "Usage: ./IMGPushClient [OPTION]..."  << endl;
	cout << "Starts a client that pushes images to the server every 1 second\n" << endl;

	cout << "Command Line Options: \n" << endl;
	cout << "\t--server\tServer IP Address (default localhost)\n" << endl;
	cout << "\t--image\t\tPath to image to push (default ./foo.jpg)\n" << endl;
}

int main(int argc, char* argv[]){

	std::string addr = "localhost"; //Default Server Address
	std::string image = "./foo.jpg"; //Default Image Location

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

	//Check if image exists
	if(FILE *file = fopen(image.c_str(), "r")){
		fclose(file);
	}
	else{
		cout << "Image not found!" << endl;
		IMGPushClient::usage();
		return -1;
	}

	cout << "Running with following parameters: \n" << endl;
	cout << "Server Address: " << addr << endl;
	cout << "Image Location: " << image << endl;

	IMGPushClient* ipc = new IMGPushClient(addr, image);
	ipc->run();

	cout << "Press any key to exit. \n\n" << endl;
	getchar();

	return 0;
}
