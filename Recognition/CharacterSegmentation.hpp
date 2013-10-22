#ifndef CHARACTERSEGMENTATION_H
#define CHARACTERSEGMENTATION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class CharacterSegmentation : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
