#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Saliency/Saliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

void Stub_Saliency :: execute(imgdata_t *data){
	setDone(data, STUB_SALIENCY);
	cout << "Stub Saliency" << endl;
}
