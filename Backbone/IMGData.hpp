#ifndef IMGDATA_H_
#define IMGDATA_H_

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
	std::vector<unsigned char> *image_data;
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
	bool orthorectDone;
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

    imgdata_t *next;

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

		orthorectDone(false),
		saliencyDone(false),
		segDone(false),
		sDone(false),
		cDone(false),
		verified(false),
		
		shape(""),
		character(""),
		scolor(""),
		ccolor(""),

		planelat(-1.0),
		planelongt(-1.0),
		planealt(-1.0),
		planeheading(-1.0),
		targetlat(-1.0),
		targetlongt(-1.0),
		targetorientation(-1.0),

        next(nullptr)
		{}
};

void img_print(imgdata_t* data);
void setDone(imgdata_t *data, AlgClass alg);
void initEmptyIMGData(imgdata_t *data);
void clearIMGData(imgdata_t *data);
void copyIMGData(imgdata_t *dest, imgdata_t *src);

#endif
