#ifndef ____SHARED_UTILS_H____
#define ____SHARED_UTILS_H____


#include <exception>
#include <string>
#include <sstream>
#include <assert.h>
#include <vector>
#include <typeinfo>
#include <cmath>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CV_WAIT_KEY_OPTIONAL 0



class OutputMessageHandler
{
protected:
    //choose this at compile time; for final release builds, use 0 or 1
    unsigned char AcceptableOutputLevel;

    //junk stream where unwanted text goes to die
    std::ostream unprinted_output_stops_here;

public:
    void SetAcceptableOutputLevel(unsigned char newlevel) {AcceptableOutputLevel = newlevel;}

    OutputMessageHandler();
    OutputMessageHandler(const OutputMessageHandler & copyFrom);
    OutputMessageHandler& operator= (const OutputMessageHandler &copyFrom);

    //get a stream to write the output to
    //the message handler will decide if it wants to print at that level
    //
    std::ostream& Level(int given_level);
    std::ostream& Level0(); //you can use this for runtime errors, or just use std::cout
    std::ostream& Level1(); //interesting stuff that doesn't fill much space in the console
    std::ostream& Level2(); //gap level for stuff you may be currently testing
    std::ostream& Level3(); //more debugging stuff, that takes up more space in the console
    std::ostream& Level4(); //testing debug stuff that may fill up a ton of space in the console
};
std::ostream& operator<< (std::ostream& stream, const OutputMessageHandler& msg);

extern OutputMessageHandler consoleOutput;



template <class ptrToType>
void DeletePointersInVectorAndClearIt(std::vector<ptrToType> & t)
{
	typename std::vector<ptrToType>::iterator myiter = t.begin();
	
	for(; myiter != t.end(); myiter++) {
		delete (*myiter);
	}
	t.clear();
}



class myexception : public std::exception
{
public:

    std::string my_string;

    virtual const char* what() const throw()
    {
        return my_string.c_str();
    }

    myexception() : std::exception() {}
    myexception(std::string MY_STRING) : std::exception(), my_string(MY_STRING) {}
};



int RoundFloatToInteger(float num);
int RoundDoubleToInteger(double num);
double ModulusD(double num, double divisor);

double GetMeanAngle(std::vector<double> & the_angles);



template <class T>
inline std::string to_istring(const T& t)
{
	std::stringstream ss;
	ss << static_cast<int>(t);
	return ss.str();
}
template <class T>
inline std::string to_sstring(const T& t)
{
	std::stringstream ss;
	ss << (t);
	return ss.str();
}
inline std::string char_to_string(const char & input)
{
	char tempstr[2];
	tempstr[0] = input;
	tempstr[1] = 0;
	return std::string(tempstr);
}

#ifndef MIN
#define MIN(xxxxx,yyyyy) ((xxxxx) < (yyyyy) ? (xxxxx) : (yyyyy))
#endif
#ifndef MAX
#define MAX(xxxxx,yyyyy) ((xxxxx) > (yyyyy) ? (xxxxx) : (yyyyy))
#endif
#ifndef CLAMP
#define CLAMP(xxxxx,minnnnn,maxxxxx) (MIN(MAX((xxxxx),(minnnnn)),(maxxxxx)))
#endif

#ifndef __stricmp

#ifndef WIN32
#include <strings.h>
#define __stricmp(x,y) strcasecmp(x,y)
#else
#pragma message("building for Windows")
#define __stricmp(x,y) _stricmp(x,y)
#endif

#endif


std::string GetPathOfExecutable(const char* argv0);

bool contains_substr_i(std::string theStr, std::string containsThis, int* position=nullptr);
bool str_contains_char(std::string theStr, char containsThis, int* position=nullptr);
std::string replace_substr_in_string(std::string inputstr, std::string old_str_to_be_replaced, std::string new_str);
std::string replace_char_in_string(std::string inputstr, char old_char_to_be_replaced, char new_char);

std::string fixPathRedundancies(std::string path_to_fix);

std::string get_chars_before_delim(const std::string & thestr, char delim);

//returns the chars after the LAST INSTANCE OF THE delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end);

//returns the chars after the FIRST INSTANCE OF THE delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_first_instance_of_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end);

std::string get_extension_from_filename(const std::string & filename);
std::string eliminate_extension_from_filename(std::string & filename);

bool filename_extension_is_image_type(const std::string & filename_extension);

bool check_if_file_exists(const std::string & filename);
bool check_if_directory_exists(const std::string & dir_name);

void DeleteFilesOfTypeInFolder(std::string folder, std::string filename_extension);

void TryPrintAllFileNamesInFolder(std::string folder_dir_name, std::ostream &PRINT_HERE);
int CountNumImagesInFolder(std::string folder_dir_name);

std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

//For debugging; get the name of what the object is; e.g. "myClass" or "std::vector<int>"
std::string demangle_typeid_name(const char* name);

template <class T>
std::string get_typeid_name_of_class(const T& t) {

    return demangle_typeid_name(typeid(t).name());
}

//Helper functions and variables for trig functions
const double kPI = 4 * atan(1);
double to_degrees(double radians);
double to_radians(double degrees);

std::string ConvertOrientationToString(double orientation);

//expects values to be normalized to the range 0 to 255
std::vector<float> ConvertOneColorPoint(float r, float g, float b, int new_colorspace__from_bgr_to_this);

#endif
