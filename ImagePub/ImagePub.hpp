#ifndef IMAGEPUB_H
#define IMAGEPUB_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class ImagePub : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
