#ifndef DISPLAY_VERIF_H
#define DISPLAY_VERIF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class DisplayVerify : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
