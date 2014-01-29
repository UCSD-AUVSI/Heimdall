#ifndef IMGDATA_H
#define IMGDATA_H

#include <vector>
#include <string>

#include "Backbone/Backbone.hpp"

struct imgdata_t{
	int id;

	std::vector<unsigned char> *image_data = 0;
	
	//Do not use these integers, only for packing/unpacking
	int image_data_size = 0;

	//TODO: Change this to a algclass_t:boolean mapping
	//so that we can arbitrarily add/remove algs
	//without having to change these	
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
void freeIMGData(imgdata_t *data);

#endif
