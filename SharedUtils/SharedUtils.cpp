/**
 * @file Utils_SharedUtils.cpp
 * @brief Misc utility functions, like mathematics, string formatting. Includes console output system with varying levels of verbosity.
 * @author Jason Bunk
 */

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/GlobalVars.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <unistd.h>
#ifndef _getcwd
#define _getcwd getcwd
#endif

//--------------------------------------------------------------------------
// Global Variables, from "GlobalVars.hpp"

/*extern*/ std::string* path_to_HeimdallBuild_directory = nullptr;
/*extern*/ std::string* path_to_HClient_executable = nullptr;

/*extern*/ ExperimentResultsData* globalLatestExperimentResults = nullptr;
/*extern*/ int globalNumImagesInExperiment = 0;
/*extern*/ int globalExperimentNum = 0;

/*extern*/ void (*globalExperimentResultsCalculatorFunc)(std::vector<imgdata_t*>, ExperimentResultsData*) = nullptr;
//--------------------------------------------------------------------------


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
std::ostream& OutputMessageHandler::Level(int given_level)
{
	if(AcceptableOutputLevel >= given_level)
		return std::cout;
	return unprinted_output_stops_here;
}
//-------------------------------------


int RoundFloatToInt(float num)
{
	if ((num - floor(num)) < 0.5f)
		return ((int)floor(num));
	else
		return ((int)ceil(num));
}
int RoundDoubleToInt(double num)
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


std::string GetPathOfExecutable(const char* argv0)
{
	std::string fullpath;
	std::string argv0str = replace_char_in_string(argv0, '\\', '/');
	if(argv0str[0] == '/') {
		fullpath = argv0str; //launched using absolute path
	}
	else {
		char cwd[4096];		//launched using relative path
		memset(&(cwd[0]),0,4096);
		if(_getcwd(cwd, sizeof(cwd)) == nullptr) {
			std::cout<<"WARNING: getcwd() error... possibly executable directory is way too long?"<<std::endl;
			return "";
		} else {
			fullpath = std::string(cwd) + std::string("/") + argv0str;
		}
	}
	return fixPathRedundancies(fullpath);
}


bool contains_substr_i(std::string theStr, std::string containsThis, int* position/*=nullptr*/)
{
	int imax = (theStr.size() - containsThis.size() + 1);
	for(int i=0; i<imax; i++) {
		if(!__stricmp(theStr.substr(i,containsThis.size()).c_str(), containsThis.c_str())) {
			if(position != nullptr) {
				(*position) = i;
			}
			return true;
		}
	}
	if(position != nullptr) {
		(*position) = -1;
	}
	return false;
}


bool str_contains_char(std::string theStr, char containsThis, int* position/*=nullptr*/)
{
	for(int ii=0; ii<theStr.size(); ii++) {
		if(theStr[ii] == containsThis) {
			if(position != nullptr) {
				(*position) = ii;
			}
			return true;
		}
	}
	if(position != nullptr) {
		(*position) = -1;
	}
	return false;
}


std::string replace_substr_in_string(std::string inputstr, std::string old_str_to_be_replaced, std::string new_str) {
	if(old_str_to_be_replaced.empty())
		return std::string();
	size_t start_pos = 0;
	while((start_pos = inputstr.find(old_str_to_be_replaced, start_pos)) != std::string::npos) {
		inputstr.replace(start_pos, old_str_to_be_replaced.length(), new_str);
		start_pos += new_str.length(); // In case 'new_str' contains 'old_str', like replacing 'x' with 'yx'
	}
	return inputstr;
}


std::string replace_char_in_string(std::string inputstr, char old_char_to_be_replaced, char new_char) {
	for(int ii=0; ii<inputstr.size(); ii++) {
		if(inputstr[ii] == old_char_to_be_replaced) {
			inputstr[ii] = new_char;
		}
	}
	return inputstr;
}


std::string fixPathRedundancies(std::string path_to_fix)
{
	int foundpos;
	while(contains_substr_i(path_to_fix, "../", &foundpos)) {
		while((path_to_fix.size() > (foundpos+3)) && path_to_fix.substr(foundpos+3,3) == "../") {
			foundpos += 3;
		}
		int foundpos2 = path_to_fix.find_first_of('/',foundpos+3);
		if(foundpos2 != path_to_fix.npos) {
			path_to_fix.erase(foundpos,(foundpos2-foundpos+1));
		}
		else {
			break;
		}
	}
	while(contains_substr_i(path_to_fix,"/./")) {
		path_to_fix = replace_substr_in_string(path_to_fix,"/./","/");
	}
	while(contains_substr_i(path_to_fix,"//")) {
		path_to_fix = replace_substr_in_string(path_to_fix,"//","/");
	}
	return path_to_fix;
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

//returns the chars after the FIRST INSTANCE OF THE delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_first_instance_of_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end)
{
	size_t lastDelimPos = thestr.find_first_of(delim);
	if(lastDelimPos > 0 && lastDelimPos < thestr.size()) {
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


bool read_file_contents(std::string filename, std::string & returnedFileContents)
{
	returnedFileContents.clear();
	std::ifstream myfile(filename);
	if(myfile.is_open() && myfile.good()) {
		std::string line;
		while(std::getline(myfile,line)) {
			returnedFileContents = (returnedFileContents + std::string("\n") + line);
		}
		myfile.close();
		return true;
	}
	return false;
}


bool check_if_file_exists(std::string filename)
{
	std::ifstream myfile(filename);
	if(myfile.is_open() && myfile.good()) {
		myfile.close();
		return true;
	}
	return false;
}


bool check_if_directory_exists(std::string dir_name)
{
	return dir_name.empty()==false && check_if_file_exists(dir_name);
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

std::vector<std::string> GetImageFilenamesInFolder(std::string folder_dir_name)
{
	std::vector<std::string> returnedFnames;
	tinydir_dir dir;
	tinydir_open(&dir, folder_dir_name.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.' && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			returnedFnames.push_back(file.name);
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	return returnedFnames;
}

void DeleteFilesOfTypeInFolder(std::string folder, std::string filename_extension)
{
	std::string thisfname;
	std::string thisextens;
	tinydir_dir dir;
	tinydir_open(&dir, folder.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.') {
			thisfname = std::string(file.name);
			//std::cout<<"found file (might delete?) "<<thisfname<<std::endl;
			thisextens = trim_chars_after_delim(thisfname,'.',true);
			//std::cout<<"      extension: \""<<thisextens<<"\""<<std::endl;
			if(!__stricmp(thisextens.c_str(),filename_extension.c_str())) {
				thisfname = std::string(file.path);
				//std::cout<<"DELETING \""<<thisfname<<"\""<<std::endl;
				std::remove(thisfname.c_str());
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
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

