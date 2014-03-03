#ifndef IMGDATA_H
#define IMGDATA_H

#include <vector>
#include <string>

#include "Backbone/Backbone.hpp"

struct imgdata_t{
	uint32_t id, cropid = 0;
	bool initialized = false;

	// image_data vector guaranteed to have only one row when packed/unpacked
	std::vector<std::vector<unsigned char>*> *image_data = 0;
	std::vector<std::vector<unsigned char>*> *sseg_image_data = 0;
	std::vector<std::vector<unsigned char>*> *cseg_image_data = 0;
	
	//Do not use these, only for packing/unpacking/internal ops
	uint32_t image_data_size = 0;
	std::vector<uint32_t> *sseg_image_sizes = 0;
	std::vector<uint32_t> *cseg_image_sizes = 0;
	uint32_t sseg_image_size_count = 0;
	uint32_t cseg_image_size_count = 0;

	// TODO: Change this to a algclass_t:boolean mapping
	// so that we can arbitrarily add/remove algs
	// without having to change these	
	bool orgrDone = false;
	bool saliencyDone = false;
	bool segDone = false;
	bool sDone = false;
	bool cDone = false;
	bool verified = false;

	std::string shape = "", character = "";
	std::string scolor = "", ccolor = "";

	double planelat = 0.0, planelongt = 0.0, planealt = 0.0, planeheading = 0.0;
	double targetlat = 0.0, targetlongt = 0.0;
	double targetorientation = 0.0;
};

void img_print(imgdata_t* data);
void setDone(imgdata_t *data, alg_t alg);
void initEmptyIMGData(imgdata_t *data);
void clearIMGData(imgdata_t *data);

#endif
