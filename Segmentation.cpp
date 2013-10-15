#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"

void segExec(imgdata_t &data){
	data.csegDone = true;
	data.ssegDone = true;
	cout << "Segmentation" << endl;
}
