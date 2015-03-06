
#include "skynet_VisionUtil.hpp"
#include "SharedUtils/SharedUtils.hpp"


namespace Skynet {


ManagedIPL::ManagedIPL(std::string path, int options)
{
	img = cvLoadImage(path.c_str(), options);
}

ManagedIPL::~ManagedIPL()
{
	IplImage *localImg = img;
	cvReleaseImage(&localImg);
}

std::string shapeFloatToString(float input)
{
	int in = ((int)input);
	std::string retVal;
	switch (in)
	{
	case 0:
		retVal = "Squa";
		break;
	case 1:
		retVal = "Squa";//"Diamond";
		break;
	case 2:
		retVal = "Rect";
		break;
	case 3:
		retVal = "Para";
		break;
	case 4:
		retVal = "Hexa";
		break;
	case 5:
		retVal = "Circ";
		break;
	case 6:
		retVal = "Pent";
		break;
	case 7:
		retVal = "Semi"; //semi circle
		break;
	case 8:
		retVal = "Star";
		break;
	case 9:
		retVal = "Trap";
		break;
	case 10:
		retVal = "Cros";
		break;
	case 11:
		retVal = "Tria";
		break;
	case 12:
		retVal = "Quar"; //quarter circle
		break;
	default:
		retVal = "Unknown";
		break;
	}

	return retVal;//.tolower();
}



// return an int. 100 == unknown
// if you modify this, modify shapeFloatToString also
int shapeStrToInt(std::string const shape)
{
	std::string theShape = shape;//.tolower();
	if (theShape == "squa" || theShape == "diam") //
		return 1; // square == diamond
	if (theShape == "rect") //
		return 2;
	if (theShape == "para") //
		return 3;
	if (theShape == "hexa")
		return 4;
	if (theShape == "circ") //
		return 5;
	if (theShape == "pent")
		return 6;
	if (theShape == "semi") //
		return 7;
	if (theShape == "star")
		return 8;
	if (theShape == "trap") //
		return 9;
	if (theShape == "cros") //
		return 10;
	if (theShape == "tria") //
		return 11;
	if (theShape == "quar") //
		return 12;

	throw myexception("Unknown Shape: " + shape);
}


std::string parseFilenameForLetter(std::string const filename)
{
	std::string letter = filename.substr(0,1);//.tolower();
	letter = disambiguateLetter(letter);
	return letter;
}

std::string parseFilenameForShape(std::string const filename)
{
	return filename.substr(2,4);//.tolower();
}

std::string parseFilenameForSeparateShape(std::string const filename)
{
	return filename.substr(0,4);//.tolower();
}

std::string disambiguateLetter(std::string input)
{
	if (input.empty())
		return "";

	if (input == "o")
		input = "0";

	if (input == "6")
		input = "9";

	return input;
}

PixelColor blackPixelColor()
{
	PixelColor color;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	return color;
}

float calcColorDistance(cv::Vec<float, 3> color1, cv::Vec<float, 3> color2)
{
	float deltaSquaredSum = 0.0f;
	for (int i = 0; i < 3; ++i)
	{
		float delta = (color1[i] - color2[i]);
		deltaSquaredSum += (delta*delta);
	}
	return sqrt(deltaSquaredSum);
}
double calcColorDistance(cv::Scalar color1, cv::Scalar color2)
{
	double deltaSquaredSum = 0.0;
	for (int i = 0; i < 3; ++i)
	{
		double delta = (color1[i] - color2[i]);
		deltaSquaredSum += (delta*delta);
	}
	return sqrt(deltaSquaredSum);
}


cv::MatND calcHistogramOfImage(const cv::Mat& img)
{
	int defaultNumBins = 20;
	return calcHistogramOfImage(img, defaultNumBins);
}

cv::MatND calcHistogramOfImage(const cv::Mat& img, int numBins)
{
	return calcHistogramOfImage(img, numBins, cv::Mat());
}

cv::MatND calcHistogramOfImage(const cv::Mat& img, int numBins, const cv::Mat& mask)
{
	cv::MatND hist;
	int channels[] = {0, 1, 2};
	int histSize[] = {numBins, numBins, numBins};
    float channelRanges[] = { 0, MAX_RGB_COLOR_VALUE+1 };
    const float* ranges[] = { channelRanges, channelRanges, channelRanges };
	cv::calcHist(&img, 1, channels, mask, hist, img.channels(), histSize, ranges);

	return hist;
}

cv::Mat magnitude(const cv::Mat& img)
{
	std::vector<cv::Mat> imgChannels;
	cv::split(img, imgChannels);

	int numChannels = 3;
	cv::Mat sumSquaredTotal = cv::Mat::zeros(img.size(), CV_32FC1);
	for (int i = 0; i < numChannels; ++i)
	{
		cv::Mat chn = imgChannels[i];
		cv::multiply(chn, chn, chn);
		cv::add(chn, sumSquaredTotal, sumSquaredTotal);
	}
	cv::Mat magnitude;
	cv::sqrt(sumSquaredTotal, magnitude);

	return magnitude;
}

cv::Mat outsideFill(cv::Mat& input, unsigned char color)
{
	uchar differentColor = (color == 128 ? 1 : 128);
	cv::Mat filled = input.clone();
	cv::floodFill(filled, cv::Point(0,0), cv::Scalar(differentColor));
	cv::Mat changedPixels = filled - input;
	input.setTo(color, changedPixels == 0);
	return input;
}

cv::Mat centerFill(const cv::Mat& input, unsigned char color)
{
	cv::Mat filled = input.clone();
	cv::floodFill(filled, cv::Point(input.cols/2,input.rows/2), cv::Scalar(color));
	return filled;
}

float farthestPixelFromCenter(const cv::Mat& input, unsigned char color)
{
	cv::Point center = cv::Point(input.cols/2, input.rows/2);
	cv::Mat masked = input == color;
	float farthestDistance = 0.0f;
	for (int x = 0; x < input.cols; ++x)
	{
		for (int y = 0; y < input.rows; ++y)
		{
			bool pixelIsOn = masked.at<cv::Vec<uchar, 1>>(y,x)[0] > 0;
			if (pixelIsOn)
			{
				float thisDistance = pythagoreanDistance((float)x-center.x, (float)y-center.y);
				farthestDistance = std::max(farthestDistance, thisDistance);
			}
		}
	}
	return farthestDistance;
}

float radians(float deg)
{
	return (float)(deg*M_PI/180.0f);
}

float degrees(float rad)
{
	return (float)(rad*180.0f/M_PI);
}

float distanceBetweenAngles(float angleARadians, float angleBRadians)
{
	return distanceBetweenAngles(angleARadians, angleBRadians, (float)M_PI);
}

float distanceBetweenAngles(float angleARadians, float angleBRadians, float overflowLocationRadians)
{
	angleARadians = shiftAngleToPositive(angleARadians, overflowLocationRadians);
	angleBRadians = shiftAngleToPositive(angleBRadians, overflowLocationRadians);
	float angleDistance = abs(angleARadians - angleBRadians);
	if (angleDistance > overflowLocationRadians/2)
	{
		// try rotating along unit circle, since 0 degrees and 179 degrees should be close
		angleDistance = (float)abs(angleDistance - overflowLocationRadians);
	}
	return angleDistance;
}

#define VERY_LOW_NUMBER -1e6 // is -1,000,000
#define VERY_LARGE_NUMBER 1e6 // is 1,000,000

float shiftAngleToPositive(float angleRadians, float overflowLocationRadians)
{
	if (angleRadians < VERY_LOW_NUMBER)
		return 0.0f;
	if (angleRadians > VERY_LARGE_NUMBER)
		return 0.0f;
	while (angleRadians < 0.0f)
		angleRadians += overflowLocationRadians;
	while (angleRadians >= overflowLocationRadians)
		angleRadians -= overflowLocationRadians;
	return angleRadians;
}

float pythagoreanDistance(float x, float y)
{
	return std::sqrt(std::pow(x,2) + std::pow(y,2));
}



float ratioOfBlobToWhole(const cv::Mat& img)
{
	int sizeOfImg = img.rows * img.cols;
	int sizeOfShape = cv::countNonZero(img);
	return sizeOfShape/(float)sizeOfImg;
}

}
