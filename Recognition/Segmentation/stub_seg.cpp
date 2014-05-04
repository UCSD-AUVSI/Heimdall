#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "stub_seg.hpp"

using std::cout;
using std::endl;

void StubSeg :: execute(imgdata_t *imdata, std::string args) {
	cout << "Stub Segmentation" << endl;
	setDone(imdata, SEG);
}
