#include <string>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Orthorect/Orthorect.hpp"

using std::cout;
using std::endl;

void Stub_Orthorect :: execute(imgdata_t *data, std::string args){
	setDone(data, ORTHORECT);
	cout << "Stub Orthorect" << endl;
}
