#include <string>
#include <iostream>
#include <fstream>
#include <thread>

#include "opencv2/opencv.hpp"

#include "EmergentSearch/PSSub.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

using std::cout;
using std::endl;

std::string kOutputFolder = "psin";

void PSSub :: DoWork() {
	cout << "PSSub" << endl;
    while (true) {
        imgdata_t* imdata = GetNext();
        cout << "Got " << imdata->id << ", " << imdata->cropid << endl;

        std::string filename = kOutputFolder + "/" + 
            std::to_string(imdata->id) + "_" + 
            std::to_string(imdata->cropid);


        cv::imwrite(filename + ".jpg", cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR));
    
        std::ofstream out;
        out.open(filename + ".txt");
        out << imdata->qrCodeMessage << endl;
        out << imdata->shape << endl;
        out << imdata->scolor << endl;
        out << imdata->character << endl;
        out << imdata->ccolor << endl;
        out << imdata->targetlat << endl;
        out << imdata->targetlongt << endl;
        out << imdata->targetorientation << endl;
        out.close();

        delete imdata;
    }
}

void usage(std::string default_server = "localhost") {
    cout << "Usage: ./PSSub [OPTION]..."  << endl;
    cout << "Starts a subscriber client that pulls saliency images and saves them to disk" << endl;

    cout << "Command Line Options: \n" << endl;
    cout << "\t--server\t\tServer IP Address\n" << endl;
    cout << "Default Server Address: " << default_server << endl;
}

int main(int argc, char* argv[]) {
    std::string default_server("localhost");

    if (argc != 1 && argc != 3) {
        usage();
        return 0;
    } else if (argc == 3) {
        if (std::string(argv[1]) == "--server") {
            default_server = std::string(argv[2]);
        } else {
            usage();
            return 0;
        }
    }

    PSSub psworker(default_server, VERIF);
    
    std::thread workThread(&PSSub::DoWork, &psworker);
    workThread.detach();

    cout << "Press any key to quit" << endl;
    getchar();
    
    return 0;
}
