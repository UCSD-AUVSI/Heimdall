#include <string>
#include <vector>
#include <iostream>

#include "Saliency/Saliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

using std::cout;
using std::endl;

void Stub_Saliency :: execute(imgdata_t *imdata, std::string args){
	setDone(imdata, SALIENCY);
	cout << "Stub Saliency" << endl;
}
