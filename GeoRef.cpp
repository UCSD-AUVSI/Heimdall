#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"

void geoRefExec(imgdata_t &data){
	data.georefDone = true;
	cout << "GeoRef" << endl;
}
