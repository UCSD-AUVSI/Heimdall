#ifndef VERIF_H
#define VERIF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class Verify : Algorithm {
	public:
		static void execute(imgdata_t &data);
};

#endif
