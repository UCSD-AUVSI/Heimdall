#ifndef PLANE_VERIF_H
#define PLANE_VERIF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class PlaneVerify : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data, std::string args);
    private:
        static void usage();
        static void processArguments(std::string args, std::string& folder);
};

#endif
