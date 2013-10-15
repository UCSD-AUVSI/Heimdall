#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"

void orthorectExec(imgdata_t &data){
	data.orthorectDone = true;
	cout << "OrthoRect" << endl;
}
