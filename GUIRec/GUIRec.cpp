#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "GUIRec/GUIRec.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void GUIRec :: execute(imgdata_t &data){
	setDone(data, SSEG);
	setDone(data, CSEG);
	setDone(data, SREC);
	setDone(data, OCR);
	cout << "GUI Recognition" << endl;
}
