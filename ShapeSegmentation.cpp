#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void ShapeSegmentation :: execute(imgdata_t &data){
	setDone(data, SSEG);
	cout << "Shape Seg" << endl;	
}
