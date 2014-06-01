#ifndef FOLDERPUSH_H
#define FOLDERPUSH_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class FolderPush : AUVSI_Algorithm{
    public:
        static void initialize();
        static void execute(imgdata_t *imdata, std::string args);
    private:
        static int sendcount, delay;
        static bool send, pause, search_subfolders, first_send;
        static std::vector<std::string> * file_list;
        static void usage();
        static void processArguments(std::string args, std::string& folder);
        static int FindAllImagesInDir(std::string dirpath, int subdir_recursion_depth_limit);
};

#endif
