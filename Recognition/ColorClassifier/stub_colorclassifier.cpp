#include <iostream>

using std::cout;
using std::endl;

#include "stub_colorclassifier.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void StubColorClassifier :: execute(imgdata_t *imdata, std::string args) {
    imdata->scolor = "unknown";
    imdata->ccolor = "unknown";
	cout << "Stub Color Classifier" << endl;	
	setDone(imdata, COLORCLASS);
}
