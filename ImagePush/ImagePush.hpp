#ifndef IMAGEPUSH_H
#define IMAGEPUSH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class ImagePush : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
