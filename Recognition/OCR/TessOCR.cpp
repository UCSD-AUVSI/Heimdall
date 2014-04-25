#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/OCR/TessOCR.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void TessOCR :: execute(imgdata_t *data, std::string args){
	setDone(data, OCR);
	cout << "Tesseract OCR" << endl;
}
