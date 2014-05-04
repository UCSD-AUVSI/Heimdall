/**
 * @file Utils_SharedUtils.cpp
 * @brief Misc utility functions, like mathematics, string formatting. Includes console output system with varying levels of verbosity.
 * @author Jason Bunk
 */

#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include <math.h>
#include <iostream>
#include <fstream>


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


bool filename_extension_is_image_type(const std::string & filename_extension)
{
    /*
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
