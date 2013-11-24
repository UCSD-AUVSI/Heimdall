#include <string>
#include <map>
#include <vector>

#include <iostream>
#include <stdexcept>

#include <zmq.hpp>

#include "Backbone/Backbone.hpp"
#include "Backbone/Maps.hpp"
#include "Backbone/DistClient.hpp"

using std::cout;
using std::endl;

int main(int argc, char* argv[]){
	int i = 1;

	std::string addr = "localhost"; //Default Server Address

	std::string alg;
	std::vector<std::string> alglist;

	std::map<std::string, alg_t> local_strMap;

	for(auto& x: selectMap){
		local_strMap.insert(std::pair<std::string, alg_t>(x.first, strMap.at(x.second[0])));
	}

	for(int i = 1; i < argc; i++){
		if(argv[i] == "--server"){
			if(++i >= argc){
				DistClient::usage();
				return -1;
			}
			addr = argv[i];
		}
		else{
			try{
				std::string algtype = std::string(argv[i]);
				algtype = algtype.substr(2); //Strip off two dashes

				alglist = selectMap.at(algtype);
				
				if(++i >= argc){
					DistClient::usage();
					return -1;
				}
				alg = std::string(argv[i]);
				
				if(std::find(alglist.begin(), alglist.end(), alg) == alglist.end()){
					DistClient :: usage();
					return -1;
				}

				local_strMap[algtype] = strMap.at(alg);
			}	
			catch(const std::out_of_range &oor){
				DistClient :: usage();
				return -1;
			}
		}
	}

	for(auto& x: local_strMap){
		if(x.second != NONE){
			DistClient* dc = new DistClient(addr, algMap.at(x.second), portMap.at(x.second));
			dc->run();	
		}
	}

	cout << "Press any key to exit." << endl;
	getchar();

	return 0;
}
