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

std::string ConvertColorToString(double r, double g, double b) {
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
    
    double distance = 450;
    std::string color = "";

    for(auto &x: color_map){
        double curr_dist = sqrt( 
                pow(x.second[0] - r, 2) +
                pow(x.second[1] - g, 2) +
                pow(x.second[2] - b, 2));

        if (curr_dist < distance) {
            distance = curr_dist;
            color = x.first;
        }
    }

    return color;
}
