#include <string>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Orthorect/stub_orthorect.hpp"

using std::cout;
using std::endl;

void StubOrthorect :: execute(imgdata_t *data, std::string args){
	setDone(data, ORTHORECT);
	cout << "Stub Orthorect, ID: " << data->id  << endl;
}
