#ifndef DISPLAY_H
#define DISPLAY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Display : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
