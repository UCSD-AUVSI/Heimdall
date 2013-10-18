#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void ShapeRecognition :: execute(imgdata_t &data){
	setDone(data, SREC);
	cout << "Shape Rec" << endl;	
}
