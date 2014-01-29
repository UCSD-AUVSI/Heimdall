#ifndef GUIREC_H
#define GUIREC_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class GUIRec : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
