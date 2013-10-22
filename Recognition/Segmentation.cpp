#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/Segmentation.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void Segmentation :: execute(imgdata_t &data){
	setDone(data, CSEG);
	setDone(data, SSEG);
	cout << "Segmentation" << endl;
}
