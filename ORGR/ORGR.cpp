#include <string>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "ORGR/ORGR.hpp"

using std::cout;
using std::endl;

void Stub_ORGRF :: execute(imgdata_t *data){
	setDone(data, STUB_ORGR);
	cout << "Stub ORGR" << endl;
}
