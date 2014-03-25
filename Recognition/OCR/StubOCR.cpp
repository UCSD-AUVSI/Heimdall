#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/OCR/StubOCR.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void StubOCR :: execute(imgdata_t *data) {
	cout << "Stub OCR" << endl;
	setDone(data, STUB_OCR);
}
