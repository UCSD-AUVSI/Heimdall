
#include "skynet_HistogramSegmenter.hpp"
#include "skynet_VisionUtil.hpp"
#include "skynet_ColorBlob.hpp"


#if 1
/*extern*/ float Skynet::COLOR_DISTANCE_THRESHOLD = 100.0f;
/*extern*/ float Skynet::MERGE_COLOR_DISTANCE = 30.0f; // approx one bin
/*extern*/ int Skynet::NUM_VALID_COLORS = 8;
/*extern*/ int Skynet::NUM_BINS = 10; // was 20 - 98% ugly
/*extern*/ float Skynet::MINIMUM_BLOB_SIZE_THRESHOLD = 0.015f; //depends how well Saliency can crop around the target...
                                                //sometimes the target is small within the crop
#elif 0
/*extern*/ float Skynet::COLOR_DISTANCE_THRESHOLD = 95.0f;
/*extern*/ float Skynet::MERGE_COLOR_DISTANCE = 31.0f; // approx one bin
/*extern*/ int Skynet::NUM_VALID_COLORS = 8;
/*extern*/ int Skynet::NUM_BINS = 12; // was 20 - 98% ugly
#else
/*extern*/ float Skynet::COLOR_DISTANCE_THRESHOLD = 100.0f;
/*extern*/ float Skynet::MERGE_COLOR_DISTANCE = 35.0f; // approx one bin
/*extern*/ int Skynet::NUM_VALID_COLORS = 8;
/*extern*/ int Skynet::NUM_BINS = 11; // was 20 - 98% ugly
#endif


using namespace Skynet;


HistogramSegmenter::HistogramSegmenter(void)
{
	//numBins = NUM_BINS;
	//binsToRGBRatio = (float) (numBins / MAX_RGB_COLOR_VALUE);
}
HistogramSegmenter::~HistogramSegmenter()
{
}


std::vector<ColorBlob*>
HistogramSegmenter::findBlobs(cv::Mat colorImg, cv::Mat* returned_binned_mat)
{
    binsToRGBRatio = static_cast<float>(NUM_BINS) / MAX_RGB_COLOR_VALUE;
	setImageWithPreprocessing(colorImg);
	std::vector<ColorBlob*> blobList = findBlobWithHistogram(returned_binned_mat);
	return blobList;
}

void
HistogramSegmenter::setImageWithPreprocessing(cv::Mat colorImg)
{
	switch(colorImg.channels())
	{
	case 1:
		{
			if (colorImg.type() != CV_32FC1)
				colorImg.convertTo(colorImg, CV_32FC1);
		}
		break;
	case 2:
		{
			if (colorImg.type() != CV_32FC2)
				colorImg.convertTo(colorImg, CV_32FC2);
		}
		break;
	default: //case 3:
		{
			if (colorImg.type() != CV_32FC3)
				colorImg.convertTo(colorImg, CV_32FC3); //valgrind doesn't like this (570KB"definitely lost" AND 1.26MB"possibly lost")
		}
		break;
	}

	cv::normalize(colorImg, colorImg, 0.0, MAX_RGB_COLOR_VALUE, cv::NORM_MINMAX);
	//cv::GaussianBlur(colorImg, colorImg, cv::Size(3,3), 0);
	cv::blur(colorImg, colorImg, cv::Size(2,2));

	mImg = cv::Mat(colorImg);
}


cv::Mat
HistogramSegmenter::reduceColorsWithBinning()
{
	throw myexception("Don't use this! I just made it to prototype something");
	cv::Mat binnedImage = (mImg * binsToRGBRatio);
	binnedImage.convertTo(binnedImage, CV_32SC3);
	binnedImage.convertTo(binnedImage, CV_32FC3);
	binnedImage /= binsToRGBRatio;

	return binnedImage;
}


std::vector<ColorBlob*>
HistogramSegmenter::findBlobWithHistogram(cv::Mat* returned_binned_mat)
{
	cv::Mat input(mImg);

	cv::MatND hist = calcHistogramOfImage(input, NUM_BINS);
	std::vector<PixelColor> validColors = biggestColorsInHistogram(hist, NUM_VALID_COLORS, input.channels());
	std::vector<ColorBlob*> blobList = segmentImageIntoBlobsWithColors(input, validColors);

	if(returned_binned_mat != nullptr)
	{
		*returned_binned_mat = redrawImageWithColors(input, validColors);
	}

	return blobList;
}
std::vector<PixelColor>
HistogramSegmenter::biggestColorsInHistogram(cv::MatND hist, int numColors, int channels_of_src_img)
{
	std::vector<PixelColor> colors(numColors);
	for (int i = 0; i < numColors; ++i)
	{
		PixelColor biggestRemainingColor = getAndRemoveBiggestColorInHistogram(hist, channels_of_src_img);

		colors[i] = PixelColor(biggestRemainingColor);
	}
	colors = mergeCloseColors(colors);
	return colors;
}


PixelColor
HistogramSegmenter::getAndRemoveBiggestColorInHistogram(cv::MatND hist, int channels_of_src_img)
{
	int idxOfBiggest[3];
	getBiggestIndexInHist(hist, idxOfBiggest);

	if(channels_of_src_img <= 2)
		idxOfBiggest[2] = 0;
	if(channels_of_src_img <= 1)
		idxOfBiggest[1] = 0;

	zeroOutBinAtIndex(hist, idxOfBiggest, channels_of_src_img);
	PixelColor colorOfBin = convertBinToColor(idxOfBiggest);
	return colorOfBin;
}

void
HistogramSegmenter::getBiggestIndexInHist(cv::MatND hist, int idxOfMax[3])
{
	double maxVal;
	cv::minMaxIdx(hist, 0, &maxVal, 0, idxOfMax);
}

void
HistogramSegmenter::zeroOutBinAtIndex(cv::MatND hist, int index[3], int channels_of_src_img)
{
	switch(channels_of_src_img)
	{
	case 1:
		hist.at<float>(index[0]) = 0.0f;
		break;
	case 2:
		hist.at<float>(index[0], index[1]) = 0.0f;
		break;
	case 3:
		hist.at<float>(index[0], index[1], index[2]) = 0.0f;
		break;
	}
}

std::vector<PixelColor>
HistogramSegmenter::mergeCloseColors(std::vector<PixelColor>& colors)
{
	std::vector<PixelColor> newColors;

	for(std::vector<PixelColor>::iterator citer = colors.begin();
        citer != colors.end(); citer++)
	{
		bool foundMatch = false;

        for(std::vector<PixelColor>::iterator nciter = newColors.begin();
            nciter != newColors.end(); nciter++)
        {
			if (calcColorDistance(*citer, *nciter) < MERGE_COLOR_DISTANCE)
				foundMatch = true;
        }

		if(!foundMatch)
			newColors.push_back(*citer); //MRef destructor crashes here
	}
	return newColors;
}

PixelColor
HistogramSegmenter::convertBinToColor(int idxOfBiggest[3])
{
	PixelColor color;
	for (int i = 0; i < 3; ++i)
		color[i] = ((float)idxOfBiggest[i])/binsToRGBRatio;
	return color;
}

cv::Mat
HistogramSegmenter::redrawImageWithColors(cv::Mat& input, std::vector<PixelColor>& validColors)
{
	PixelColor inputColor;
	PixelColor convertedColor;

	Pixel2ChannelColor convertedColor2D;

	switch(input.channels())
	{
	case 2:
		inputColor[2] = 0.0f;
		for (cv::MatIterator_<Pixel2ChannelColor> it = input.begin<Pixel2ChannelColor>();
			 it != input.end<Pixel2ChannelColor>(); ++it)
		{
			inputColor[0] = (*it)[0];
			inputColor[1] = (*it)[1];

			convertedColor = convertToValidColor(inputColor, validColors);
			convertedColor2D[0] = convertedColor[0];
			convertedColor2D[1] = convertedColor[1];

			*it = convertedColor2D;
		}
		break;
	case 3:
		for (cv::MatIterator_<PixelColor> it = input.begin<PixelColor>();
			 it != input.end<PixelColor>(); ++it)
		{
			inputColor = *it;
			convertedColor = convertToValidColor(inputColor, validColors);
			//("clr: " + convertedColor[0] + "," + convertedColor[1] + "," + convertedColor[2]);

			*it = convertedColor;
		}
		break;
	}

	return input;
}

PixelColor
HistogramSegmenter::convertToValidColor(PixelColor inputColor, std::vector<PixelColor>& validColors)
{
	PixelColor closestColor;
	float bestDistance = COLOR_DISTANCE_THRESHOLD + 10.0f;

	//for each (PixelColor mCandidate in validColors)
	for(std::vector<PixelColor>::iterator vciter = validColors.begin();
            vciter != validColors.end(); vciter++)
	{
		PixelColor candidate = (*vciter);

		//std::cout<<("candidate: " + candidate[0] + "," + candidate[1] + "," + candidate[2]);

		float distance = calcColorDistance(inputColor, candidate);
		bool candidateIsBetter = distance < bestDistance;
		if (candidateIsBetter)
		{
			closestColor = candidate;
			bestDistance = distance;
		}
	}
	bool colorIsValid = bestDistance <= COLOR_DISTANCE_THRESHOLD;
	if (colorIsValid)
		return closestColor;
	else
		return blackPixelColor();
}

std::vector<ColorBlob*>
HistogramSegmenter::segmentImageIntoBlobsWithColors(cv::Mat& input, std::vector<PixelColor>& validColors)
{
	std::vector<ColorBlob*> blobList = makeBlobsWithColors(validColors);
	drawImageIntoBlobs(input, blobList);

	return blobList;
}

std::vector<ColorBlob*>
HistogramSegmenter::makeBlobsWithColors(std::vector<PixelColor>& colors)
{
	std::vector<ColorBlob*> blobs;
	int index = 1;
	//for each (PixelColor mColor in colors)
	for(std::vector<PixelColor>::iterator citer = colors.begin(); citer != colors.end(); citer++)
	{
		PixelColor color = (*citer);
		//valgrind doesn't like this ("definitely lost")
		ColorBlob * blob = new ColorBlob(index++, mImg.size());
		blob->setBlobColor(color);
		blobs.push_back(blob);
	}
	return blobs;
}

void
HistogramSegmenter::drawImageIntoBlobs(cv::Mat& input, std::vector<ColorBlob*>& blobs)
{
	PixelColor color_3channel;
	Pixel2ChannelColor color2;

	switch(input.channels())
	{
	case 2:
		color_3channel[2] = 0.0f;
		for (int x = 0; x < input.cols; ++x)
		{
			for (int y = 0; y < input.rows; ++y)
			{
				Pixel2ChannelColor color2 = input.at<Pixel2ChannelColor>(y,x);
				color_3channel[0] = color2[0];
				color_3channel[1] = color2[1];
				drawPixelIntoBlobs(cv::Point(x,y), color_3channel, blobs);
			}
		}
		break;
	case 3:
		for (int x = 0; x < input.cols; ++x)
		{
			for (int y = 0; y < input.rows; ++y)
			{
				color_3channel = input.at<PixelColor>(y,x);
				drawPixelIntoBlobs(cv::Point(x,y), color_3channel, blobs);
			}
		}
		break;
	}
}

void
HistogramSegmenter::drawPixelIntoBlobs(cv::Point pt, PixelColor pixelColor, std::vector<ColorBlob*>& blobs)
{
	ColorBlob * blob = nullptr;
	float bestDistance = COLOR_DISTANCE_THRESHOLD + 10.0f;
	//for each (ColorBlob * candidate in blobs)
	for(std::vector<ColorBlob *>::iterator candidate_iter = blobs.begin(); candidate_iter != blobs.end(); candidate_iter++)
	{
		float distance = calcColorDistance(pixelColor, (*candidate_iter)->getBlobColor());
		bool candidateIsBetter = distance < bestDistance;
		if (candidateIsBetter)
		{
			blob = (*candidate_iter);
			bestDistance = distance;
		}
	}
	bool blobIsValid = (blob != nullptr  &&  bestDistance <= COLOR_DISTANCE_THRESHOLD);
	if (blobIsValid)
		blob->addPoint(pt);
}

cv::Mat
HistogramSegmenter::secondSegmentation(PixelColor color)
{
	cv::Mat input(mImg);
	cv::Mat distanceFromShape;
	cv::add(input, -color, distanceFromShape);
	distanceFromShape = magnitude(distanceFromShape);

	cv::Mat shape;
	cv::threshold(distanceFromShape, shape, COLOR_DISTANCE_THRESHOLD, 255.0, cv::THRESH_BINARY_INV);
	shape = outsideFill(shape, 255);
	return shape;
}
