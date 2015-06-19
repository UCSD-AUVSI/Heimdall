#include <iostream>

using std::cout;
using std::endl;

#include "stub_ocr.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void StubOCR :: execute(imgdata_t *imdata, std::string args) {
    imdata->character1 = args;
	cout << "Stub OCR" << endl;
	setDone(imdata, OCR);
}
