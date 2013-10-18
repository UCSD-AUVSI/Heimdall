#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void GeoRef :: execute(imgdata_t &data){
	setDone(data, GEOREF);
	cout << "GeoRef" << endl;
}
