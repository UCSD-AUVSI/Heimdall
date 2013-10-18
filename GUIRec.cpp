#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void GUIRec :: execute(imgdata_t &data){
	setDone(data, SSEG);
	setDone(data, CSEG);
	setDone(data, SREC);
	setDone(data, OCR);
	cout << "GUI Recognition" << endl;
}
