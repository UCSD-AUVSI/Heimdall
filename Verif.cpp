#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"

void verifExec(imgdata_t &data){
	data.verified = true;
	cout << "Verification" << endl;
}
