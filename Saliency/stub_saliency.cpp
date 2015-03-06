#include <string>
#include <vector>
#include <iostream>

#include "stub_saliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

using std::cout;
using std::endl;

void StubSaliency :: execute(imgdata_t *imdata, std::string args){
	setDone(imdata, SALIENCY);
	cout << "Stub Saliency" << endl;
}
