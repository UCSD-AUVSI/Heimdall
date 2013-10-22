#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "OGRSaliency/Orthorect.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void Orthorect :: execute(imgdata_t &data){
	setDone(data, ORTHORECT);
	cout << "OrthoRect" << endl;
}
