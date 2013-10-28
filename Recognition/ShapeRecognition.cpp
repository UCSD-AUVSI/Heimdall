#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/ShapeRecognition.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void ShapeRecognition :: execute(imgdata_t &data){
	setDone(data, SREC);
	cout << "Shape Rec" << endl;	
}
