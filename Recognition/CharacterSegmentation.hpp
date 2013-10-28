#ifndef CHARACTERSEGMENTATION_H
#define CHARACTERSEGMENTATION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class CharacterSegmentation : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
