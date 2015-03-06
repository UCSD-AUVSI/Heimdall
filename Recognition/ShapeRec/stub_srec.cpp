#include <iostream>

using std::cout;
using std::endl;

#include "stub_srec.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void StubSRec :: execute(imgdata_t *imdata, std::string args){
    imdata->shape = args;
	cout << "Stub Shape Rec" << endl;	
	setDone(imdata, SREC);
}
