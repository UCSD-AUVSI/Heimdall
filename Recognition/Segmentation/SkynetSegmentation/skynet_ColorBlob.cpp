
#include "skynet_ColorBlob.hpp"
#include "skynet_VisionUtil.hpp"
#include <cmath>
#include "skynet_BlobResult.hpp"

using namespace Skynet;



ColorBlob::ColorBlob(BlobId bId, cv::Size sizeIn)
{
	id = bId;
	size = new cv::Size();
	size->width = sizeIn.width;
	size->height = sizeIn.height;
	mask = new cv::Mat(*size, CV_8UC1);
	mask->setTo(0);
	blobColor = NULL;
	setBlobColor(blackPixelColor());

	stats = new ColorBlobStatistics(this);

#if 1
    saved_last_fraction_of_blobs_pixels_along_border = -1.0f;
#endif
}

ColorBlob::~ColorBlob()
{
	delete size;
	delete mask;
	if (blobColor != NULL)
		delete blobColor;
}

bool
ColorBlob::hasSameColor(ColorBlob * blob, const cv::Mat& img)
{
	//calculateBlobColor(img);
	//blob->calculateBlobColor(img);

	PixelColor thisColor = stats->getMostCommonColor(img, area());//mostCommonColor(img);
	PixelColor otherColor = blob->stats->getMostCommonColor(img, blob->area());//->mostCommonColor(img);

	float colorDistance = calcColorDistance(thisColor, otherColor);
	return colorDistance <= MaxColorDistanceForMerge;
}

PixelColor
ColorBlob::mostCommonColor(const cv::Mat& img)
{
	bool blobIsVerySmall = area() < 5.1f;
	if (blobIsVerySmall)
	{
		return getARandomPixel(img);
	}
	else
	{
		cv::MatND hist = calcHistogram(img);
		return getHistogramPeak(hist);
	}
}

PixelColor
ColorBlob::getARandomPixel(const cv::Mat& img)
{
	assert(true); //not sure if this function is needed for the histogram-based segmentation


	int maxIdx[2];
	cv::minMaxIdx(*mask, 0, 0, 0, maxIdx);
	PixelColor color = img.at<PixelColor>(maxIdx[0], maxIdx[1]);
	PixelColor equivalentHistogramLocation;
	for (int i = 0; i < 3; ++i)
		equivalentHistogramLocation[i] = (float)std::floor(color[i]*NUM_HISTOGRAM_BINS/MAX_RGB_COLOR_VALUE);

	return equivalentHistogramLocation;
}

cv::MatND
ColorBlob::calcHistogram(const cv::Mat& img)
{
	return calcHistogramOfImage(img, NUM_HISTOGRAM_BINS, *mask);
}

PixelColor
ColorBlob::getHistogramPeak(cv::MatND hist)
{
	double maxVal;
	int maxIdx[3];
	cv::minMaxIdx(hist, 0, &maxVal, 0, maxIdx);
	PixelColor color;
	for (int i = 0; i < 3; ++i)
		color[i] = (float)maxIdx[i];

	return color;
}

void
ColorBlob::expandToContainBlob(ColorBlob * blob, cv::Mat *blobIds)
{
	cv::bitwise_or(*mask, *(blob->mask), *mask);
	bool blobIdsIsValid = blobIds != NULL;//.size().width != 0;
	if (blobIdsIsValid)
	{
		BlobId newId = id;
		blobIds->setTo(newId, *(blob->mask));
	}

	stats->invalidate();
}

void
ColorBlob::calculateBlobColor(const cv::Mat& img)
{
	cv::Scalar newBlobColor = cv::mean(img, *mask);
	(*blobColor)[0] = (float)newBlobColor[0];
	(*blobColor)[1] = (float)newBlobColor[1];
	(*blobColor)[2] = (float)newBlobColor[2];
}

void
ColorBlob::drawIntoImg(cv::Mat& img)
{
	img.setTo(*blobColor, *mask);
}

void
ColorBlob::drawIntoFalseColorImg(cv::Mat& img, FalseColor color)
{
	if(img.channels() == 3)
		img.setTo(color, *mask);
	else if(img.channels() == 1)
	{
		FalseColor_1channel fc1c;
		fc1c[0] = color[0];
		img.setTo(fc1c, *mask);
	}
	else if(img.channels() == 2)
	{
		FalseColor_2channel fc2c;
		fc2c[0] = color[0];
		fc2c[1] = color[1];
		img.setTo(fc2c, *mask);
	}
}
void
ColorBlob::drawFilledIntoFalseColorImg(cv::Mat& img, unsigned char color)
{
	cv::Mat filled = mask->clone();

    if(filled.type() == CV_8U) //should always be true
    {

        //If it's a black pixel, do flood fill. This algorithm won't work if you start flood filling from a white pixel.
        //it's possible for (0,0) to be a white pixel, since a few edge-touching pixels is acceptable.
        //(algorithm: do flood fill outside the shape; then everything that wasn't filled by this, is the shape)

        if(filled.at<unsigned char>(0,0) == 0)
            cv::floodFill(filled, cv::Point(0,0), 128);
        else
        {
            for(int j=1; j<filled.cols; j++)
            {
                //scan across the top edge; this will work.
                //  if the WHOLE top edge was filled, the blob wouldn't have been accepted.

                if(filled.at<unsigned char>(0,j) == 0) //(y,x) access notation
                {
                    cv::floodFill(filled, cv::Point(j,0), 128); //(x,y) access notation
                    break;
                }
            }
        }

        cv::Mat outsideShape = filled - *mask;
        img.setTo(color, outsideShape == 0);
    }
    else
        drawIntoFalseColorImg(img, FalseColor(color,color,color));
}

void
ColorBlob::addPoint(cv::Point pt, cv::Mat blobIds)
{
	mask->at<MaskElement>(pt.y, pt.x)[0] = 1;
	bool blobIdsIsValid = blobIds.size().width != 0;
	if (blobIdsIsValid)
	{
		blobIds.at<BlobIdElement>(pt.y, pt.x)[0] = id;
	}
	stats->invalidate();
}


float
ColorBlob::CalculateTotalPerimeters()
{
    float retval = 0.0f;

    if(mask != nullptr)
    {
        std::vector<std::vector<cv::Point>> FoundContours;

        cv::Mat newmask;
        mask->copyTo(newmask); //cv::findContours modifies the Mat it is passed, so don't let it modify our blob mask!

        cv::findContours(newmask, FoundContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));

        //Add up all the perimeters of all of the found contours/lines/blobs
        if(FoundContours.empty()==false)
        {
            for(std::vector<std::vector<cv::Point>>::iterator iter = FoundContours.begin();
                                iter != FoundContours.end(); iter++)
            {
                retval += cv::arcLength(*iter, true);
            }
        }
    }
	return retval;
}


#define FOUR_PI_float 12.566370614359173f

//
//1 for a perfect circle,
//0.785 for a square,
//0.5 for two perfect circles side-by-side
//          (the more circles, the less total circularity; so dust specs in a blob reduce its "circularity")
//0.310 for a rectangle of sides 1x8
//0 for an ideal line
//
//noisiness also decreases circularity by increasing the perimeter
//

float
ColorBlob::CalculateCircularity()
{
    float Area = area();
    float perimeter = CalculateTotalPerimeters();

    return ((FOUR_PI_float * Area) / (perimeter*perimeter));
}


float
ColorBlob::area()
{
	return (float)cv::sum(*mask)[0];
}

void
ColorBlob::setBlobColor(PixelColor color)
{
	if (blobColor != NULL)
		delete blobColor;

	blobColor = new PixelColor(color);
}

//Compare the fraction of the blob's pixels that belong to a border
bool
ColorBlob::isInterior(float acceptable_fraction_of_blobs_pixels_that_touch_edge)
{
	cv::Mat edgeMask = cv::Mat(mask->size(), CV_8UC1);
	edgeMask.setTo(0);
	edgeMask.col(0).setTo(1);
	edgeMask.row(0).setTo(1);
	edgeMask.col(mask->cols-1).setTo(1);
	edgeMask.row(mask->rows-1).setTo(1);
	cv::Mat valuesOnEdges;
	mask->copyTo(valuesOnEdges, edgeMask);
	int numPixelsOnBorder_FromBlob = cv::countNonZero(valuesOnEdges);

    float numPixelsInBlob = area();

#if 0
	float fraction_ofBlobPixels_OnBorder =
            (static_cast<float>(numPixelsOnBorder_FromBlob) / numPixelsInBlob);

consoleOutput.Level3() << std::string("ColorBlob::isInterior() - numOnBorder: ") << to_sstring(numPixelsOnBorder_FromBlob) << std::endl;
consoleOutput.Level3() << std::string("ColorBlob::isInterior() - area: ") << to_sstring(numPixelsInBlob) << std::endl;
consoleOutput.Level3() << std::string("ColorBlob::isInterior() - fractionOnBorder: ") << to_sstring(fraction_ofBlobPixels_OnBorder) << std::endl;
#endif
#if 1
    saved_last_fraction_of_blobs_pixels_along_border = (static_cast<float>(numPixelsOnBorder_FromBlob) / numPixelsInBlob);
#endif

	return (static_cast<float>(numPixelsOnBorder_FromBlob) / numPixelsInBlob) < acceptable_fraction_of_blobs_pixels_that_touch_edge;
}

cv::Mat
ColorBlob::clonedMaskWithNoSmallBlobs(int minimum_num_pixels_in_speck)
{
	cv::Mat clonedMask = mask->clone();

	CBlobResult blobs;
	CBlob * currentBlob;
	IplImage binaryIpl = clonedMask;
	blobs = CBlobResult( &binaryIpl, NULL, 0 );

	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, minimum_num_pixels_in_speck );
	for (int i = 0; i < blobs.GetNumBlobs(); i++ )
	{
    	currentBlob = blobs.GetBlob(i);
		currentBlob->FillBlob( &binaryIpl, cvScalar(0));
	}

	//clonedMask = binaryIpl;

	return clonedMask;
}

void
ColorBlob::EliminateTinyNoiseSpeckles(int minimum_num_pixels_in_speck)
{
	cv::Mat bigBlobMask = clonedMaskWithNoSmallBlobs(minimum_num_pixels_in_speck);
	mask->release();
	(*mask) = bigBlobMask;
}

unsigned char
ColorBlob::maskValueAt(cv::Point pt)
{
	unsigned char maskValue = mask->at<MaskElement>(pt.y, pt.x)[0];
	return maskValue;
}

void
ColorBlob::runTests()
{
	ColorBlob *testFixture = new ColorBlob(5, cv::Size(10,10));
	testFixture->testSameColor();

	testFixture = new ColorBlob(5, cv::Size(10,10));
	testFixture->testCalcBlobColor();

	testFixture = new ColorBlob(5, cv::Size(10,10));
	testFixture->testAddPoint();

	testFixture = new ColorBlob(5, cv::Size(10,10));
	testFixture->testExpandToContainBlob();
}

void
ColorBlob::testSameColor()
{
	cv::Size imgSize = cv::Size(10,10);
	cv::Mat img = cv::Mat::zeros(imgSize, CV_8UC3);

	/*ColorBlob *secondFixture = new ColorBlob(6, cv::Size(10,10));
	secondFixture->blobColor[0] = 2.0f;
	secondFixture->blobColor[1] = 0.5f;
	secondFixture->blobColor[2] = 1.0f;

	if (!hasSameColor(secondFixture))
		throw myexception("ERROR");

	blobColor[0] = 200.0f;

	if (hasSameColor(secondFixture))
		throw myexception("ERROR");*/
}

void
ColorBlob::testExpandToContainBlob()
{
	// setup
	cv::Size imgSize = cv::Size(10,10);
	cv::Mat blobIds = cv::Mat::zeros(imgSize, CV_32SC1);
	ColorBlob *secondFixture = new ColorBlob(6, imgSize);
	secondFixture->addPoint(cv::Point(1,0), blobIds);
	addPoint(cv::Point(0,0), blobIds);

	expandToContainBlob(secondFixture, &blobIds);

	// check mask
	bool maskIsZero = mask->at<MaskElement>(0, 1)[0] == 0;
	if (maskIsZero)
		throw myexception("ERROR");

	maskIsZero = mask->at<MaskElement>(0, 0)[0] == 0;
	if (maskIsZero)
		throw myexception("ERROR");

	// check blobIds
	BlobId idAtSecondBlob = blobIds.at<BlobIdElement>(0, 1)[0];
	bool blobIdsIsCorrect = idAtSecondBlob == id;
	if (!blobIdsIsCorrect)
		throw myexception("ERROR");

	idAtSecondBlob = blobIds.at<BlobIdElement>(0, 0)[0];
	blobIdsIsCorrect = idAtSecondBlob == id;
	if (!blobIdsIsCorrect)
		throw myexception("ERROR");


}

void
ColorBlob::testCalcBlobColor()
{
	cv::Mat img(*size, CV_32FC3);
	img.setTo(5.0f);

	addPoint(cv::Point(0,0));
	calculateBlobColor(img);

	PixelColor correctColor;
	correctColor[0] = 5.0f;
	correctColor[1] = 5.0f;
	correctColor[2] = 5.0f;

	bool blobColorIsIncorrect = calcColorDistance(*blobColor, correctColor) > 0.001f;
	if (blobColorIsIncorrect)
		throw myexception("ERROR");
}

void
ColorBlob::testAddPoint()
{
	cv::Mat blobIds(*size, CV_32SC1);
	cv::Point pt(0,0);
	bool maskIsZero = mask->at<MaskElement>(pt.y, pt.x)[0] == 0;
	if (!maskIsZero)
		throw myexception("ERROR");

	addPoint(pt, blobIds);

	maskIsZero = mask->at<MaskElement>(pt.y, pt.x)[0] == 0;
	if (maskIsZero)
		throw myexception("ERROR");

	bool blobIdsAtProperValue = blobIds.at<BlobIdElement>(pt.y, pt.x)[0] == id;
	if (!blobIdsAtProperValue)
		throw myexception("ERROR");
}

PixelColor
ColorBlobStatistics::getMostCommonColor(const cv::Mat& img, float area)
{
	float thresholdRatio = 0.03f; // 3 percent is a good number
	float changeInArea = abs(area-cachedArea);
	bool areaHasChangedSignificantly = cachedArea < 0.0f || changeInArea > cachedArea*thresholdRatio;
	if (areaHasChangedSignificantly)
	{
		cachedArea = area;
		mostCommonColor = new MRef<PixelColor>( new PixelColor(parent->mostCommonColor(img)));
		setWasUpdated();
	}

	return *(mostCommonColor->obj);
}
