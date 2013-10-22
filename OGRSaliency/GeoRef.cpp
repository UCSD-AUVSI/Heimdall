#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "OGRSaliency/GeoRef.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void GeoRef :: execute(imgdata_t &data){
	setDone(data, GEOREF);
	cout << "GeoRef" << endl;
}
