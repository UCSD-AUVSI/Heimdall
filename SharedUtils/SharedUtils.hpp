#ifndef ____SHARED_UTILS_H____
#define ____SHARED_UTILS_H____


#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>


std::string GetCoolString();


void ConvertMat_UsingSettings(cv::Mat & source_mat, cv::Mat & destination_mat, int preprocess_CV_conversion_type, bool const*const which_channels_to_keep, bool delete_unwanted_channels);

std::string GetNameOfCVColorSpace(int CV_colorspace_conversion_type);

#define COLORSPACE_CONVERSIONTYPE_KEEPRGB -1234567


cv::Mat Average_Several_SingleChannel_CVMats(std::vector<cv::Mat>* input_mats, float max_acceptable_fractional_difference=1.00001f);
cv::Scalar Average_Several_CVColors(std::vector<cv::Scalar>* input_colors);

double GetLengthOfCVScalar(cv::Scalar input);

int GetContourOfGreatestArea(std::vector<std::vector<cv::Point>> & contours,
                            double* returned_area_of_largest=nullptr,
                            double* returned_total_area=nullptr);


//----------------------------------------------------------------------

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

//this should have already been defined on Windows machines
#ifndef __stricmp

#ifndef WIN32
#include <strings.h>
#define __stricmp(x,y) strcasecmp(x,y)
#else
#define __stricmp(x,y) _stricmp(x,y)
#endif

#endif


std::string get_chars_before_delim(const std::string & thestr, char delim);

//returns the chars after the delim, maybe including the delim (determined by the boolean)
std::string trim_chars_after_delim(std::string & thestr, char delim, bool include_delim_in_returned_trimmed_end);

std::string get_extension_from_filename(const std::string & filename);
std::string eliminate_extension_from_filename(std::string & filename);

bool filename_extension_is_image_type(const std::string & filename_extension);



#endif
