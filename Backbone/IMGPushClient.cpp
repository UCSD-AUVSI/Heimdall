#include <string>
#include <map>
#include <vector>

#include <thread>
#include <iostream>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Maps.hpp"
#include "Backbone/IMGPushClient.hpp"

#include "opencv2/opencv.hpp"

using std::cout;
using std::endl;

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
		cout << "Sending " << sizeof(imgdata_t) << " bytes" << endl;
		
		zmq::message_t msg(sizeof(imgdata_t));
		data.id = count++;
		cv::imencode(".jpg", cv::imread(image), data.image_data);
		
		memcpy(msg.data(), &data, sizeof(imgdata_t));
		pushsocket.send(msg);
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
