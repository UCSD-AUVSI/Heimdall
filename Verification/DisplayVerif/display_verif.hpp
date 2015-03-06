#ifndef DISPLAY_VERIF_H
#define DISPLAY_VERIF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

#include <fstream>

extern std::fstream* outfile_verif_results;
extern bool outfile_verif_results_has_been_opened;

class DisplayVerify : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
