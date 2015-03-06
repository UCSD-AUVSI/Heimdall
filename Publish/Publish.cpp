#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <cmath>
#include <cstdlib>
#include <thread>

#include "opencv2/opencv.hpp"

#include "Publish/Publish.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "SharedUtils/SharedUtils.hpp"

using std::cout;
using std::endl;

const std::string kOutputFolder = "usboutput";
const std::string kTextFN = kOutputFolder + "/UCSDAUVSI.txt";

std::string ConvertToDMS (double decgps, bool isLat) {
    std::stringstream newgps;
    if (decgps < 0) {
        newgps << (isLat?"S":"W");
        decgps = -decgps;
    } else {
        newgps << (isLat?"N":"E");
    }

    double degrees = floor(decgps);
    newgps << std::setfill('0') << std::setw((isLat?2:3));
    newgps << (int)degrees << " ";

    decgps -= degrees;
    double minutes = floor(decgps * 60);
    newgps << std::setfill('0') << std::setw(2);
    newgps << (int)minutes << " ";

    decgps -= minutes/60;
    double seconds = decgps * 3600;
    newgps << std::setfill('0') << std::setw(2);
    newgps << floor((int)seconds) << ".";
    newgps << std::setw(3);
    newgps << (int)((seconds - floor(seconds))*1000);
    
    return newgps.str();
}

std::string ConvertToDMS (std::string decgpsstring, bool isLat) {
    return ConvertToDMS(atof(decgpsstring.c_str()), isLat);
}

std::string DefaultIfEmpty(std::string input){
    if (input.size() > 0) {
        return input;

    } else {
        return "N/A";
    }
}

void Publish :: DoWork() {
    cout << "Publish" << endl;
    std::ofstream out;
    out.open(kTextFN);

    for(int i = 1; i <= 9; i++){
        out << i << "\t";
    }
    out << endl;

    int target_counter = 1;
    while (true) {
        imgdata_t* imdata = GetNext();
        cout << "Got " << imdata->id << ", " << imdata->cropid << endl;

        std::string filename = std::to_string(imdata->id) + "_" + 
            std::to_string(imdata->cropid) + ".jpg";

        // Save Crop
        cv::imwrite(kOutputFolder + "/" + filename, cv::imdecode(*(imdata->image_data), CV_LOAD_IMAGE_COLOR));

        // Write target number
        out << std::setfill('0') << std::setw(2);
        out << target_counter++ << "\t";

        // Lat/Long
        out << ConvertToDMS(imdata->targetlat, true) << "\t";
        out << ConvertToDMS(imdata->targetlongt, false) << "\t";

        // Orientation
        out << std::setw(2);
        out << ConvertOrientationToString(imdata->targetorientation) << "\t";

        // Shape + Color
        out << DefaultIfEmpty(imdata->shape) << "\t";
        out << DefaultIfEmpty(imdata->scolor) << "\t";

        // Character + Color
        out << DefaultIfEmpty(imdata->character) << "\t";
        out << DefaultIfEmpty(imdata->ccolor) << "\t";

        // Image file name
        out << filename;

        out << endl;
        out.flush();

        delete imdata;
    }
    out.close();
}

void usage(std::string default_server = "localhost") {
    cout << "Usage: ./Publish [OPTION]..."  << endl;
    cout << "Starts a subscriber client that pulls verified images and saves them to disk" << endl;

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
        cout << std::string(argv[1]) << endl;
        if (std::string(argv[1]) == "--server") {
            default_server = std::string(argv[2]);
        } else {
            usage();
            return 0;
        }
    }

    Publish pubworker(default_server, VERIF);

    std::thread workThread(&Publish::DoWork, &pubworker);
    workThread.detach();

    cout << "Press any key to quit" << endl;
    getchar();

    return 0;
}
