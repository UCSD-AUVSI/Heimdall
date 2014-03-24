
#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>


namespace Skynet {

	#define MAX_RGB_COLOR_VALUE 256.0
	#define SIZE_OF_FEATURE_VECTOR 320

	#define PTSTR(pt) "(" + pt.x + "," + pt.y + ")"
	#define VEC2STR(pt) "(" + pt[0] + "," + pt[1] + ")"

	//typedef unsigned char uchar;
	typedef cv::Vec<float, 1> Pixel1ChannelColor;
	typedef cv::Vec<float, 2> Pixel2ChannelColor;
	typedef cv::Vec<float, 3> PixelColor;


	// wraps an IplImage, so you don't have to worry about memory management
	class ManagedIPL
	{
	public:
		ManagedIPL(std::string path, int options);
		~ManagedIPL();

		IplImage *img;
	};

	// wraps any allocated native object, deleting it automatically when gc runs
	// this is here because native objects cannot be a part of managed classes
	// so, use this to wrap your native objects and don't worry about memory leaks!
	// :)
	template<typename T> class MRef
	{
	public:
		MRef(T o) { obj = new T(o); }
		MRef(T *o) { obj = o; }

		T o() { return *obj; } // returns raw object (not pointer to object)
		T* obj; // pointer to object
	protected:
		~MRef() { delete obj; }
	};

	// cache statistics easily. subclass this and override updateStatistics
	class CachedStatistics
	{
	public:
		CachedStatistics()
		{
			isValid = false;
		}

		void invalidate()
		{
			isValid = false;
		}

		void updateIfNecessary()
		{
			if (needsUpdating())
				updateStatistics();
			setWasUpdated();
		}

		void updateStatistics() {}

	protected:
		bool needsUpdating() { return !isValid; }
		void setWasUpdated() { isValid = true; }
		bool isValid;
	};

	std::string shapeFloatToString(float input);
	int shapeStrToInt(std::string const shape);

	std::string parseFilenameForLetter(std::string const filename);
	std::string parseFilenameForShape( std::string const filename);
	std::string parseFilenameForSeparateShape(std::string const filename);
	std::string disambiguateLetter(std::string input);

	PixelColor blackPixelColor();

	float  calcColorDistance(PixelColor color1, PixelColor color2);
	double calcColorDistance(cv::Scalar color1, cv::Scalar color2);

	cv::MatND calcHistogramOfImage(const cv::Mat& img);
	cv::MatND calcHistogramOfImage(const cv::Mat& img, int numBins);
	cv::MatND calcHistogramOfImage(const cv::Mat& img, int numBins, const cv::Mat& mask);

	cv::Mat magnitude(const cv::Mat& img);
	cv::Mat outsideFill(cv::Mat& input, unsigned char color);
	cv::Mat centerFill(const cv::Mat& input, unsigned char color);
	float farthestPixelFromCenter(const cv::Mat& input, unsigned char color);

	float ratioOfBlobToWhole(const cv::Mat& img);

	float radians(float deg);
	float degrees(float rad);

	float distanceBetweenAngles(float angleARadians, float angleBRadians, float overflowLocationRadians);
	float distanceBetweenAngles(float angleARadians, float angleBRadians);

	float shiftAngleToPositive(float angleRadians, float overflowLocationRadians);

	float pythagoreanDistance(float x, float y);
}
