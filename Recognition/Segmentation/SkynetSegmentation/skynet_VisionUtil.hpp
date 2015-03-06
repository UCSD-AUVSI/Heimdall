
#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "SharedUtils/SharedUtils.hpp"


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
		T o() { return *obj; } // returns raw object (not pointer to object)
		T* obj; // pointer to object


        MRef() : obj(nullptr) {}
		MRef(T o)
		{
            std::cout << "Skynet::MRef created from object of type " << get_typeid_name_of_class(obj) << std::endl;
            obj = new T(o);
        }
		MRef(T *o)
		{
            if(o != nullptr) {
            std::cout << "Skynet::MRef created from pointer to type " << get_typeid_name_of_class(obj) << std::endl;
            }
            obj = o;
        }
        //copy constructor
        MRef(MRef & copySource)
        {
            obj = copySource.obj;
            copySource.obj = nullptr; //don't let it delete it, now that I have it
        }
        //copy constructor, where copySource needs to be const
        MRef(const MRef & copySource)
        {
            std::cout << "Skynet::MRef created from object of type " << get_typeid_name_of_class(obj) << std::endl;
            //copySource is about to delete its obj, so we need to copy it so our pointer doesn't point to garbage data
            obj = new T(*copySource.obj);
        }

        //assignment operator
        MRef & operator=(const MRef &rhs)
        {
            if(this != &rhs) // Same object?
            {
                if(obj != nullptr && obj != rhs.obj){
                    std::cout << "Skynet::MRef: obj deleted (assignment operator from class) type " << get_typeid_name_of_class(obj) << std::endl;
                    delete obj;
                }
                obj = rhs.obj;
            }
            return *this;
        }
        //assignment operator
        MRef & operator=(T *o)
        {
            if(obj != nullptr && obj != o){
                std::cout << "Skynet::MRef says: obj deleted (assignment operator from ptr) type " << get_typeid_name_of_class(obj) << std::endl;
                delete obj;
            }
            obj = o;
            return *this;
        }
		~MRef()
		{
            if(obj != nullptr) {
                std::cout << "Skynet::MRef says: obj deleted (destructor), type " << get_typeid_name_of_class(obj) << std::endl;
                delete obj;
            }
        }
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
