#ifndef IMGDATA_H
#define IMGDATA_H

#include <vector>
#include <string>

#include "Backbone/Backbone.hpp"

#ifndef uint32_t
#include <stdint.h>
#endif


struct imgdata_t{
	uint32_t id, cropid;
	bool initialized;

	// image_data vector guaranteed to have only one row when packed/unpacked
	std::vector<std::vector<unsigned char>*> *image_data;
	std::vector<std::vector<unsigned char>*> *sseg_image_data;
	std::vector<std::vector<unsigned char>*> *cseg_image_data;
	
	//Do not use these, only for packing/unpacking/internal ops
	uint32_t image_data_size;
	std::vector<uint32_t> *sseg_image_sizes;
	std::vector<uint32_t> *cseg_image_sizes;
	uint32_t sseg_image_size_count;
	uint32_t cseg_image_size_count;

	// TODO: Change this to a algclass_t:boolean mapping
	// so that we can arbitrarily add/remove algs
	// without having to change these
	bool orgrDone;
	bool saliencyDone;
	bool segDone;
	bool sDone;
	bool cDone;
	bool verified;

	std::string shape, character;
	std::string scolor, ccolor;

	double planelat, planelongt, planealt, planeheading;
	double targetlat, targetlongt;
	double targetorientation;

	imgdata_t() :
		id(0),
		cropid(0),
		initialized(false),

		image_data(nullptr),
		sseg_image_data(nullptr),
		cseg_image_data(nullptr),
	
		image_data_size(0),
		sseg_image_sizes(nullptr),
		cseg_image_sizes(nullptr),
		sseg_image_size_count(0),
		cseg_image_size_count(0),

		orgrDone(false),
		saliencyDone(false),
		segDone(false),
		sDone(false),
		cDone(false),
		verified(false),
		
		shape(""),
		character(""),
		scolor(""),
		ccolor(""),

		planelat(0.0),
		planelongt(0.0),
		planealt(0.0),
		planeheading(0.0),
		targetlat(0.0),
		targetlongt(0.0),
		targetorientation(0.0)
		{}
};

void img_print(imgdata_t* data);
void setDone(imgdata_t *data, alg_t alg);
void initEmptyIMGData(imgdata_t *data);
void clearIMGData(imgdata_t *data);

#endif
