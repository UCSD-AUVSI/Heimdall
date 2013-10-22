#ifndef GUIREC_H
#define GUIREC_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class GUIRec : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
