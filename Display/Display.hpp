#ifndef DISPLAY_H
#define DISPLAY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class Display : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
