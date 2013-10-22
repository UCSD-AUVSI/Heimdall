#ifndef CHARACTERRECOGNITION_H
#define CHARACTERRECOGNITION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class CharacterRecognition : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
