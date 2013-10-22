#ifndef PUBLISH_H
#define PUBLISH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class Publish : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
