#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"

void salExec(imgdata_t &data){
	data.saliencyDone = true;
	cout << "Saliency" << endl;
}
