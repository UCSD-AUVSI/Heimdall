#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"

void sRecExec(imgdata_t &data){
	data.sDone = true;
	cout << "Shape Rec" << endl;	
}

void ocrExec(imgdata_t &data){
	data.cDone = true;
	cout << "OCR" << endl;
}
