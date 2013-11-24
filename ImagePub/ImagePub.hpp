#ifndef IMAGEPUB_H
#define IMAGEPUB_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ImagePub : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
