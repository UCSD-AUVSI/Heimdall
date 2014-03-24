#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <list>
#include "skynet_VisionUtil.hpp"
#include "SharedUtils/SharedUtils.hpp"


namespace Skynet {


	class ColorBlob;

	class Segmenter
	{
	public:
		Segmenter(void)
		{
		}

		virtual std::vector<ColorBlob *> * findBlobs(cv::Mat colorImg, cv::Mat* returned_binned_mat)
		{
			throw myexception("Tried to use Abstract Class - Use subclass instead");
			return nullptr;
		}

		std::vector<ColorBlob *> * findBlobs(cv::Mat colorImg)
		{
			return findBlobs(colorImg, nullptr);
		}

		virtual cv::Mat secondSegmentation(PixelColor color)
		{
			throw myexception("Tried to use Abstract Class - Use subclass instead");
			return cv::Mat();
		}


	};

}
