#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void Segmentation :: execute(imgdata_t &data){
	setDone(data, CSEG);
	setDone(data, SSEG);
	cout << "Segmentation" << endl;
}
