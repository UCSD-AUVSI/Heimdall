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
    std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
    newarr->push_back('B');
    imdata->image_data->push_back(newarr);
	cout << "Stub Saliency" << endl;
}
