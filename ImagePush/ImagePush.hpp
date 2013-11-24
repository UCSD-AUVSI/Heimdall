#ifndef IMAGEPUSH_H
#define IMAGEPUSH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ImagePush : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
