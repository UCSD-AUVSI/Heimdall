/**
 * @file Utils_SharedUtils.cpp
 * @brief Misc utility functions, like mathematics, string formatting. Includes console output system with varying levels of verbosity.
 * @author Jason Bunk
 */

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>


//-------------------------------------
/*extern*/ OutputMessageHandler consoleOutput = OutputMessageHandler();


OutputMessageHandler::OutputMessageHandler(const OutputMessageHandler & copyFrom) :
    AcceptableOutputLevel(copyFrom.AcceptableOutputLevel),
    unprinted_output_stops_here(nullptr)
{}
OutputMessageHandler& OutputMessageHandler::operator= (const OutputMessageHandler &copyFrom)
{
    AcceptableOutputLevel = copyFrom.AcceptableOutputLevel;
}

OutputMessageHandler::OutputMessageHandler() :
    //you can choose this at compile time... for release builds, use 0 or 1
    AcceptableOutputLevel(1),
    unprinted_output_stops_here(nullptr)
{}

std::ostream& OutputMessageHandler::Level0()
{
    return std::cout;
}
std::ostream& OutputMessageHandler::Level1()
{
	if(AcceptableOutputLevel >= 1)
		return std::cout;
	return unprinted_output_stops_here;
}
std::ostream& OutputMessageHandler::Level2()
{
	if(AcceptableOutputLevel >= 2)
		return std::cout;
	return unprinted_output_stops_here;
}
std::ostream& OutputMessageHandler::Level3()
{
	if(AcceptableOutputLevel >= 3)
		return std::cout;
	return unprinted_output_stops_here;
}
std::ostream& OutputMessageHandler::Level4()
{
	if(AcceptableOutputLevel >= 4)
		return std::cout;
	return unprinted_output_stops_here;
}
//-------------------------------------


int RoundFloatToInteger(float num)
{
	if ((num - floor(num)) < 0.5f)
		return ((int)floor(num));
	else
		return ((int)ceil(num));
}
int RoundDoubleToInteger(double num)
{
	if ((num - floor(num)) < 0.5)
		return ((int)floor(num));
	else
		return ((int)ceil(num));
}


double ModulusD(double num, double divisor)
{
	return num - (divisor * floor(num / divisor));
}


double GetMeanAngle(std::vector<double> & the_angles)
{
	double xx = 0.0;
	double yy = 0.0;

	std::vector<double>::iterator iter = the_angles.begin();
	for(; iter != the_angles.end(); iter++)
	{
        xx += cos((*iter) * 0.017453292519943296); //to radians
        yy += sin((*iter) * 0.017453292519943296);
	}
	if((fabs(xx)+fabs(yy)) < 0.000001)
		return 0.0;

	return (atan2(yy,xx) * 57.295779513082321); //back to degrees
}


std::string get_chars_before_delim(const std::string & thestr, char delim)
{
    size_t lastDelimPos = thestr.find_last_of(delim);

    if(lastDelimPos > 0 && lastDelimPos < thestr.size())
    {
        return thestr.substr(0, lastDelimPos);
    }
    return thestr;
}


//returns the chars after the delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end)
{
    size_t lastDelimPos = thestr.find_last_of(delim);

    if(lastDelimPos > 0 && lastDelimPos < thestr.size())
    {
        std::string returned_extension;

        if(include_delim_in_returned_trimmed_end)
            returned_extension = thestr.substr(lastDelimPos);
        else
            returned_extension = thestr.substr(lastDelimPos+1);

        thestr.erase(thestr.begin()+lastDelimPos, thestr.end());

        return returned_extension;
    }
    return "";
}


std::string get_extension_from_filename(const std::string & filename)
{
    size_t lastPeriodPos = filename.find_last_of('.');

    if(lastPeriodPos > 0 && lastPeriodPos < filename.size())
    {
        return filename.substr(lastPeriodPos);
    }

    return "";
}


std::string eliminate_extension_from_filename(std::string & filename)
{
    return trim_chars_after_delim(filename, '.', true);
}


bool check_if_file_exists(const std::string & filename)
{
	std::ifstream myfile(filename);
	if(myfile.is_open() && myfile.good()) {
		myfile.close();
		return true;
	}
	return false;
}


bool check_if_directory_exists(const std::string & dir_name)
{
	return check_if_file_exists(dir_name);
/*	tinydir_dir dir;
	tinydir_open(&dir, dir_name.c_str());
	if(dir.has_next)
	{
		tinydir_close(&dir);
		return true;
	}
	tinydir_close(&dir);
	return false;*/
}

void TryPrintAllFileNamesInFolder(std::string folder_dir_name, std::ostream &PRINT_HERE)
{
	tinydir_dir dir;
	tinydir_open(&dir, folder_dir_name.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.') {
			PRINT_HERE << "\"" << folder_dir_name << "\" contains:" << std::string(file.name) << std::endl;
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
}

int CountNumImagesInFolder(std::string folder_dir_name)
{
	int returned_num_images = 0;
	tinydir_dir dir;
	tinydir_open(&dir, folder_dir_name.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.' && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			returned_num_images++;
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	return returned_num_images;
}


bool filename_extension_is_image_type(const std::string & filename_extension)
{
    /*
     * Possible re-write of this function
     *
    std::string possible_extensions[14] = {".bmp", ".dib", ".jpeg",
        ".jpg", ".jpe", ".jp2", ".png", ".pbm", ".pgm", ".ppm", ".sr"
            ".ras", "tiff", ".tif"};

    std::string check_extension = filename_extension;
    std::transform(check_extension.begin(), check_extension.end(),
                    check_extension.begin(), ::tolower);
    for(std::string extension : possible_extensions){
        if(filename_extension == possible_extensions){
            return true;
        }
    }
    return false;
    */

    return (

        !__stricmp(filename_extension.c_str(), ".bmp")
    ||  !__stricmp(filename_extension.c_str(), ".dib")

    ||  !__stricmp(filename_extension.c_str(), ".jpeg")
    ||  !__stricmp(filename_extension.c_str(), ".jpg")
    ||  !__stricmp(filename_extension.c_str(), ".jpe")

    ||  !__stricmp(filename_extension.c_str(), ".jp2")

    ||  !__stricmp(filename_extension.c_str(), ".png")

    ||  !__stricmp(filename_extension.c_str(), ".pbm")
    ||  !__stricmp(filename_extension.c_str(), ".pgm")
    ||  !__stricmp(filename_extension.c_str(), ".ppm")

    ||  !__stricmp(filename_extension.c_str(), ".sr")
    ||  !__stricmp(filename_extension.c_str(), ".ras")

    ||  !__stricmp(filename_extension.c_str(), ".tiff")
    ||  !__stricmp(filename_extension.c_str(), ".tif")

    );
}

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
std::string demangle_typeid_name(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };
    return (status==0) ? res.get() : name ;
}
#else
// does nothing if not g++
std::string demangle(const char* name) {
    return name;
}
#endif

//Helper functions and variables for trig functions
double to_degrees(double radians){
    return radians * 180 / kPI;
}
double to_radians(double degrees){
    return degrees * kPI / 180;
}

std::string ConvertOrientationToString(double orientation) {
    std::map<double, std::string> orient_map = {
        {0,     "N"},
        {45,    "NE"},
        {90,    "E"},
        {135,   "SE"},
        {180,   "S"},
        {225,   "SW"},
        {270,   "W"},
        {315,   "NW"}
    };

    if (orientation < 0.0 || orientation > 360.0) {
        return "N/A";
    } else {
        double min_difference = 360.0;
        std::string retstr = "N/A";

        for(auto &x : orient_map){
            if(abs(x.first - orientation) < min_difference){
                min_difference = abs(x.first - orientation);
                retstr = x.second;
            }
        }
        return retstr;
    }
}

//---------------------------------------------------------------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
typedef cv::Vec<float, 3> MyPixelColorType;
#define Print3ElementVectorCol(INPVEC) std::string("(")<<to_sstring(INPVEC[0])<<std::string(",")<<to_sstring(INPVEC[1])<<std::string(",")<<to_sstring(INPVEC[2])<<std::string(")")


//NOTE: expects input colors to be normalized as 0-255
//it won't work well otherwise!
std::string ConvertColorToString(float r, float g, float b)
{
	std::vector<float> given_color_in_CIElab = ConvertOneColorPoint(r,g,b,CV_BGR2Lab);
	
    std::map<std::string, std::vector<float>> color_map = {
        {"Black",		{0,     0,     0    }},
        //{"Gray",		{127.5, 127.5, 127.5}},
        {"White",		{255,   255,   255  }},

        {"Red",			{200, 0,   0  }},
        {"Red",			{200, 20, 20  }},
        {"Red",			{255, 0,   0  }},
        {"Red",			{255, 20, 20  }},
        {"Dark Red",	{120, 0,   0  }},
        {"Dark Red",	{120, 20, 20  }},
        
        {"Green",		{0,   255, 0  }},
        {"Green",		{20,  255, 20 }},
        {"Green",		{0,   180, 0  }},
        {"Green",		{20,  180, 20 }},
        {"Dark Green",	{0,   120, 0  }},
        {"Dark Green",	{20,   120, 20  }},
        
        {"Blue",		{0,   0,   255}},
        {"Blue",		{20, 20,   255}},
        {"Blue",		{20, 20,   200}},
        {"Blue",		{0,   0,   200}},
        {"Dark Blue",	{0,   0,   120}},
        {"Dark Blue",	{20, 20,   120}},

        {"Yellow",		{200, 200, 20 }},
        {"Yellow",		{200, 200, 0  }},
        {"Yellow",		{255, 255, 20 }},
        {"Yellow",		{255, 255, 0  }},
        
        {"Fuchsia",		{255, 0,   255}},
        {"Aqua",		{0,   255, 255}},

        {"Orange",		{255,   127.5, 0    }},
        {"Orange",		{207,   110,  16    }},
        {"Orange",		{237,   142,  50    }},
        
        //{"Maroon",		{127.5, 0,     0    }}, //renamed "dark red"
        {"Purple",		{127.5, 0,     127.5}},
        {"Olive",		{122,   122,   0    }},
        {"Teal",		{0,     127.5, 127.5}},
    };
    
    //-----------------------------------------------
    //-----------------------------------------------
    std::map<std::string, std::vector<float>>::iterator red_iter;
    red_iter = color_map.begin();
    red_iter++;
    red_iter++;
    std::vector<float> ref_color__red = ConvertOneColorPoint(red_iter->second[0],
															 red_iter->second[1],
															 red_iter->second[2],
															 CV_BGR2Lab);
	consoleOutput.Level3() << "red (255,0,0) in CIELab == " << Print3ElementVectorCol(ref_color__red) <<std::endl;
	consoleOutput.Level3() << "given input color in CIELab == " << Print3ElementVectorCol(given_color_in_CIElab) <<std::endl;
    //-----------------------------------------------
    //-----------------------------------------------
															 
    
    
    float distance = 100000.0f;
    std::string color = "";

    for(auto &x: color_map){
		std::vector<float> ref_color_in_CIELab = ConvertOneColorPoint(x.second[0],
																	  x.second[1],
																	  x.second[2],
																	  CV_BGR2Lab);
        float curr_dist = sqrt( 
                pow(ref_color_in_CIELab[0] - given_color_in_CIElab[0], 2) +
                pow(ref_color_in_CIELab[1] - given_color_in_CIElab[1], 2) +
                pow(ref_color_in_CIELab[2] - given_color_in_CIElab[2], 2));

        if (curr_dist < distance) {
            distance = curr_dist;
            color = x.first;
        }
    }
    
    //consoleOutput.Level3() << "@@@@@@@@@@@@@@@@@@@@@@@ color given to color namer: " << std::endl << given_color_in_CIElab << std::endl;
    //consoleOutput.Level3() << to_sstring(given_color_in_CIElab.at<MyPixelColorType>(0,0)[0]) << "," << to_sstring(given_color_in_CIElab.at<MyPixelColorType>(0,0)[1]) << "," << to_sstring(given_color_in_CIElab.at<MyPixelColorType>(0,0)[2]) << std::endl;
    //consoleOutput.Level3() << "and said it was " << color << std::endl << "@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

    return color;
}

//expects values to be normalized to the range 0 to 255
std::vector<float> ConvertOneColorPoint(float r, float g, float b, int new_colorspace__from_bgr_to_this)
{
	cv::Mat input_color_in_CIElab(1, 1, CV_32FC3, 0.0);
	input_color_in_CIElab.at<MyPixelColorType>(0,0) = MyPixelColorType(b,g,r);
	
	input_color_in_CIElab = (input_color_in_CIElab * (1.0f / 255.0f)); //it expects the input to be between 0 and 255
	
	cv::cvtColor(input_color_in_CIElab, input_color_in_CIElab, new_colorspace__from_bgr_to_this); //requires 32-bit floating point
	
	return {input_color_in_CIElab.at<MyPixelColorType>(0,0)[0],
			input_color_in_CIElab.at<MyPixelColorType>(0,0)[1],
			input_color_in_CIElab.at<MyPixelColorType>(0,0)[2]};
}

 /*
    //these are RGB
    std::map<std::string, std::vector<double>> color_map = {
        {"Black",   {0, 0, 0}},
        {"White",   {255, 255, 255}},

        {"Red",     {255, 0, 0}},
        {"Green",   {0, 255, 0}},
        {"Blue",    {0, 0, 255}},

        {"Yellow",  {255, 255, 0}},
        {"Fuchsia", {255, 0, 255}},
        {"Aqua",    {0, 255, 255}},

        {"Orange",  {255, 128, 0}},
        {"Maroon",  {128, 0, 0}},
        {"Purple",  {128, 0, 128}},
        {"Olive",   {128, 128, 0}},
        {"Teal",    {0, 128, 128}},
    };
    //below are CIELab
        {"Black",	{0, 0, 0}},
        {"Gray",	{50, 0, 0}},
        {"White",	{100, 0, 0}},

        {"Red",		{53.2406, 80.0942, 67.2015}},
        {"Green",	{87.7351, -86.1813, 83.1775}},
        {"Blue",	{32.2957, 79.187, -107.862}},

        {"Yellow",	{97.1395, -21.5524, 94.4758}},
        {"Fuchsia",	{60.3235, 98.2352, -60.8255}},
        {"Aqua",	{91.1133, -48.0886, -14.131}},

        {"Orange",	{66.9565, 43.0733, 73.9576}},
        {"Maroon",	{25.4184, 47.9108, 37.9052}},
        {"Purple",	{29.6552, 58.7624, -36.3846}},
        {"Olive",	{51.6779, -12.8922, 56.5136}},
        {"Teal",	{48.0731, -28.7656, -8.45287}},
    */
