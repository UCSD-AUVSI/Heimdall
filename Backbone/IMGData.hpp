#ifndef IMGDATA_H
#define IMGDATA_H

#include "Backbone/Backbone.hpp"
#include <vector>
#include <string>

struct imgdata_t{
	int id;
	
	std::vector<unsigned char> image_data;
	std::vector<unsigned char> sseg_image_data;
	std::vector<unsigned char> cseg_image_data;

	//Change this to a alg_t:boolean mapping
	//so that we can arbitrarily add/remove algs
	//without having to change these	
	bool orgrDone = false;
	bool saliencyDone = false;
	bool segDone = false;
	bool sDone = false;
	bool cDone = false;
	bool verified = false;

	std::string shape, character;
	std::string scolor, ccolor;

	double planelat, planelongt, planealt, planeheading;
	double targetlat, targetlongt;
};

void setDone(imgdata_t *data, alg_t alg);

#endif
