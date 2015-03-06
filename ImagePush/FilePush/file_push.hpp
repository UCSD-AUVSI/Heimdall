#ifndef FILEPUSH_H
#define FILEPUSH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class FilePush : AUVSI_Algorithm{
    public:
        static void execute(imgdata_t *imdata, std::string args);
    private:
        static int sendcount;
        static bool send, pause;
        static void processArguments(std::string args, std::string& image);
        static std::vector<std::string> split(const std::string &s, char delim);
        static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
};

#endif
