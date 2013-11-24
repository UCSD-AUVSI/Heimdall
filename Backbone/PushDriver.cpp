#include <iostream>

#include "Backbone/IMGPushClient.hpp"

using std::cout;
using std::endl;

int main(int argc, char* argv[]){

	std::string addr = "localhost"; //Default Server Address
	std::string image = "./foo.jpg"; //Default Image Location

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

	IMGPushClient* ipc = new IMGPushClient(addr, image);
	ipc->run();

	cout << "Press any key to exit." << endl;
	getchar();

	return 0;
}
