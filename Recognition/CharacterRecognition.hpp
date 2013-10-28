#ifndef CHARACTERRECOGNITION_H
#define CHARACTERRECOGNITION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class CharacterRecognition :  AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t &data);
};

#endif
