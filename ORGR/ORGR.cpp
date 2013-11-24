#include <string>
#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "ORGR/ORGR.hpp"

using std::cout;
using std::endl;

void ORGRF :: execute(imgdata_t *data){
	setDone(data, ORGR);
	cout << "ORGR" << endl;
}
