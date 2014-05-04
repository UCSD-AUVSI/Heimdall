#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "GUIRec/GUIRec.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

void GUIRec :: execute(imgdata_t *data, std::string args){
	setDone(data, SEG);
	setDone(data, SREC);
	setDone(data, OCR);
	cout << "GUI Recognition" << endl;
}
