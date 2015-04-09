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
	std::string name_of_original_image_file_for_debugging;
	int num_crops_in_this_image;

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
	bool qrcDone;
	bool sDone;
	bool cDone;
	bool cclDone;
	bool verified;
	
	std::string qrCodeMessage;
	std::string shape, character;
	uint8_t scolorR, scolorG, scolorB;
	uint8_t ccolorR, ccolorG, ccolorB;
	std::string scolor, ccolor;
	
	double shapeconfidence;
	double charconfidence;
	
	double planelat, planelongt, planealt;
    double planeroll, planepitch, planeheading; // Heading: Radians
	double targetlat, targetlongt;
	double targetorientation; // Degrees

    imgdata_t *next;

	imgdata_t() :
		id(0),
		cropid(0),
		initialized(false),
		name_of_original_image_file_for_debugging(""),
		num_crops_in_this_image(0),
		
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
		qrcDone(false),
		sDone(false),
		cDone(false),
		cclDone(false),
		verified(false),
		
		scolorR(0), scolorG(0), scolorB(0),
		ccolorR(0), ccolorG(0), ccolorB(0),
		
		qrCodeMessage(""),
		shape(""),
		character(""),
		scolor(""),
		ccolor(""),
		shapeconfidence(0.0),
		charconfidence(0.0),
		
		planelat(-1.0),
		planelongt(-1.0),
		planealt(-1.0),
        planeroll(-1.0),
        planepitch(-1.0),
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
