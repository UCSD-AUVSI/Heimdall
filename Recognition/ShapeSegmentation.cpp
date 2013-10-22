#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/ShapeSegmentation.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void ShapeSegmentation :: execute(imgdata_t &data){
	setDone(data, SSEG);
	cout << "Shape Seg" << endl;	
}
