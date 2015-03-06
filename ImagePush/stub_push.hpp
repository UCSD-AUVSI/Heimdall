#ifndef STUB_PUSH_H
#define STUB_PUSH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubPush : AUVSI_Algorithm{
    public:
        static void execute(imgdata_t *imdata, std::string args);
        static bool pause;
};

#endif
