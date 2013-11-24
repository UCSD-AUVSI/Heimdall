#ifndef PUBLISH_H
#define PUBLISH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Publish : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
