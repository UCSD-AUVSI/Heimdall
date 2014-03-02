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

void Stub_Saliency :: execute(imgdata_t *imdata){
	imdata->image_data->push_back(new std::vector<unsigned char>(*(imdata->image_data->at(0))));
	imdata->image_data->push_back(new std::vector<unsigned char>(*(imdata->image_data->at(0))));
	imdata->image_data->push_back(new std::vector<unsigned char>(*(imdata->image_data->at(0))));
	imdata->image_data->push_back(new std::vector<unsigned char>(*(imdata->image_data->at(0))));
	setDone(imdata, STUB_SALIENCY);
	cout << "Stub Saliency" << endl;
}
