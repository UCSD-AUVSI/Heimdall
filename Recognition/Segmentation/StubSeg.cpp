#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Recognition/Segmentation/StubSeg.hpp"

using std::cout;
using std::endl;

void StubSeg :: execute(imgdata_t *imdata) {
	cout << "Stub Segmentation" << endl;
	setDone(imdata, STUB_SEG);
}
