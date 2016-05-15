/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "precomp_v2_4_9.hpp"
#include <iostream>

namespace cv
{
void cppCannyNearlyOriginalInternal( InputArray _inDX, InputArray _inDY,
				OutputArray _dst,
                double low_thresh, double high_thresh )
{
	Mat dx = _inDX.getMat();
	Mat dy = _inDY.getMat();
	const int srcRows = dx.rows;
	const int srcCols = dx.cols;
    CV_Assert( dx.depth() == CV_16SC1 );
    CV_Assert( dy.depth() == CV_16SC1 );
    CV_Assert( dx.size() == dy.size() );
    CV_Assert( dx.channels() == dy.channels() );
    CV_Assert( dx.channels() == 1 ); //why shouldn't this be true? why do we use "const int cn = dx.channels"?
	
    _dst.create(dx.size(), CV_8U);
    Mat dst = _dst.getMat();

    if (low_thresh > high_thresh)
        std::swap(low_thresh, high_thresh);
	
    const int cn = dx.channels(); //why is this needed? shouldn't it be == 1 always?
	
    /*Mat dx(srcRows, srcCols, CV_16SC(cn));
    Mat dy(srcRows, srcCols, CV_16SC(cn));
    Sobel(src, dx, CV_16S, 1, 0, aperture_size, 1, 0, cv::BORDER_REPLICATE);
    Sobel(src, dy, CV_16S, 0, 1, aperture_size, 1, 0, cv::BORDER_REPLICATE);*/

    //if (L2gradient) //always true
    {
        low_thresh = std::min(32767.0, low_thresh);
        high_thresh = std::min(32767.0, high_thresh);

        if (low_thresh > 0) low_thresh *= low_thresh;
        if (high_thresh > 0) high_thresh *= high_thresh;
    }
    int low = cvFloor(low_thresh);
    int high = cvFloor(high_thresh);

    ptrdiff_t mapstep = srcCols + 2;
    AutoBuffer<uchar> buffer((srcCols+2)*(srcRows+2) + cn * mapstep * 3 * sizeof(int));

    int* mag_buf[3];
    mag_buf[0] = (int*)(uchar*)buffer;
    mag_buf[1] = mag_buf[0] + mapstep*cn;
    mag_buf[2] = mag_buf[1] + mapstep*cn;
    memset(mag_buf[0], 0, /* cn* */mapstep*sizeof(int));

    uchar* map = (uchar*)(mag_buf[2] + mapstep*cn);
    memset(map, 1, mapstep);
    memset(map + mapstep*(srcRows + 1), 1, mapstep);

    int maxsize = std::max(1 << 10, srcCols * srcRows / 10);
    std::vector<uchar*> stack(maxsize);
    uchar **stack_top = &stack[0];
    uchar **stack_bottom = &stack[0];

    /* sector numbers
       (Top-Left Origin)

        1   2   3
         *  *  *
          * * *
        0*******0
          * * *
         *  *  *
        3   2   1
    */

    #define CANNY_PUSH(d)    *(d) = uchar(2), *stack_top++ = (d)
    #define CANNY_POP(d)     (d) = *--stack_top

    // calculate magnitude and angle of gradient, perform non-maxima suppression.
    // fill the map with one of the following values:
    //   0 - the pixel might belong to an edge
    //   1 - the pixel can not belong to an edge
    //   2 - the pixel does belong to an edge
    for (int i = 0; i <= srcRows; i++)
    {
        int* _norm = mag_buf[(i > 0) + 1] + 1;
        if (i < srcRows)
        {
            short* _dx = dx.ptr<short>(i);
            short* _dy = dy.ptr<short>(i);

            /*if (!L2gradient)
            {
                for (int j = 0; j < srcCols*cn; j++)
                    _norm[j] = std::abs(int(_dx[j])) + std::abs(int(_dy[j]));
            }
            else*/
            {
                for (int j = 0; j < srcCols*cn; j++)
                    _norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
            }

            if (cn > 1)
            {
                for(int j = 0, jn = 0; j < srcCols; ++j, jn += cn)
                {
                    int maxIdx = jn;
                    for(int k = 1; k < cn; ++k)
                        if(_norm[jn + k] > _norm[maxIdx]) maxIdx = jn + k;
                    _norm[j] = _norm[maxIdx];
                    _dx[j] = _dx[maxIdx];
                    _dy[j] = _dy[maxIdx];
                }
            }
            _norm[-1] = _norm[srcCols] = 0;
        }
        else
            memset(_norm-1, 0, /* cn* */mapstep*sizeof(int));

        // at the very beginning we do not have a complete ring
        // buffer of 3 magnitude rows for non-maxima suppression
        if (i == 0)
            continue;

        uchar* _map = map + mapstep*i + 1;
        _map[-1] = _map[srcCols] = 1;

        int* _mag = mag_buf[1] + 1; // take the central row
        ptrdiff_t magstep1 = mag_buf[2] - mag_buf[1];
        ptrdiff_t magstep2 = mag_buf[0] - mag_buf[1];

        const short* _x = dx.ptr<short>(i-1);
        const short* _y = dy.ptr<short>(i-1);

        if ((stack_top - stack_bottom) + srcCols > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            stack.resize(maxsize);
            stack_bottom = &stack[0];
            stack_top = stack_bottom + sz;
        }

        int prev_flag = 0;
        for (int j = 0; j < srcCols; j++)
        {
			//boundaries:   0 - 22.5,   22.5 - 67.5,   67.5 - 112.5,   112.5 - 157.5
			
            #define CANNY_SHIFT 15
			// (1<<CANNY_SHIFT) == 32768
			// TG22 == 13573
			// 0.414213562373... == arctan(22.5 degrees), it's the slope of a line of angle 22.5 degrees
            const int TG22 = (int)(0.4142135623730950488016887242097*(1<<CANNY_SHIFT) + 0.5);

            int m = _mag[j];

            if (m > low)
            {
                int xs = _x[j];
                int ys = _y[j];
                int x = std::abs(xs);
                int y = std::abs(ys) << CANNY_SHIFT;

                int tg22x = x * TG22;

                if (y < tg22x)
                {
                    if (m > _mag[j-1] && m >= _mag[j+1]) goto __ocv_canny_push;
                }
                else
                {
                    int tg67x = tg22x + (x << (CANNY_SHIFT+1));
                    if (y > tg67x)
                    {
                        if (m > _mag[j+magstep2] && m >= _mag[j+magstep1]) goto __ocv_canny_push;
                    }
                    else
                    {
                        int s = (xs ^ ys) < 0 ? -1 : 1;
                        if (m > _mag[j+magstep2-s] && m > _mag[j+magstep1+s]) goto __ocv_canny_push;
                    }
                }
            }
            prev_flag = 0;
            _map[j] = uchar(1);
            continue;
__ocv_canny_push:
            if (!prev_flag && m > high && _map[j-mapstep] != 2)
            {
                CANNY_PUSH(_map + j);
                prev_flag = 1;
            }
            else
                _map[j] = 0;
        }

        // scroll the ring buffer
        _mag = mag_buf[0];
        mag_buf[0] = mag_buf[1];
        mag_buf[1] = mag_buf[2];
        mag_buf[2] = _mag;
    }

    // now track the edges (hysteresis thresholding)
    while (stack_top > stack_bottom)
    {
        uchar* m;
        if ((stack_top - stack_bottom) + 8 > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            stack.resize(maxsize);
            stack_bottom = &stack[0];
            stack_top = stack_bottom + sz;
        }

        CANNY_POP(m);

        if (!m[-1])         CANNY_PUSH(m - 1);
        if (!m[1])          CANNY_PUSH(m + 1);
        if (!m[-mapstep-1]) CANNY_PUSH(m - mapstep - 1);
        if (!m[-mapstep])   CANNY_PUSH(m - mapstep);
        if (!m[-mapstep+1]) CANNY_PUSH(m - mapstep + 1);
        if (!m[mapstep-1])  CANNY_PUSH(m + mapstep - 1);
        if (!m[mapstep])    CANNY_PUSH(m + mapstep);
        if (!m[mapstep+1])  CANNY_PUSH(m + mapstep + 1);
    }

    // the final pass, form the final image
    const uchar* pmap = map + mapstep + 1;
    uchar* pdst = dst.ptr();
    for (int i = 0; i < srcRows; i++, pmap += mapstep, pdst += dst.step)
    {
        for (int j = 0; j < srcCols; j++)
            pdst[j] = (uchar)-(pmap[j] >> 1);
    }
}
}


static float colorspaceApproxMaxx = 1.0f; //255 for RGB, ~90 for CIELAB
//-----------------------------------------------------------------------------------------
void edge_get_dXdY_with_angles_constrained_0_to_180_deg(cv::Mat singleChannelImg, int SobelKernelSize,
													cv::Mat & returned_dX, cv::Mat & returned_dY)
{
	//singleChannelImg needs to be a single-channel floating-point image with a range from 0 to 255
	CV_Assert(singleChannelImg.type() == CV_32F);
	
	double minVal, maxVal;
	cv::Mat sfg_x, sfg_y, sfg_angle, sfg_mag;
	
	//get dX and dY first derivatives
	cv::Sobel(singleChannelImg, sfg_x, -1, 1, 0, SobelKernelSize, 1, 0, cv::BORDER_REPLICATE);
	cv::Sobel(singleChannelImg, sfg_y, -1, 0, 1, SobelKernelSize, 1, 0, cv::BORDER_REPLICATE);
	
	//get magnitude and direction of the gradient formed by those two derivatives
	cv::magnitude(sfg_x, sfg_y, sfg_mag);
	
    cv::Mat kd, ks;
    cv::getDerivKernels(kd, ks, 1, 0, SobelKernelSize, false, CV_32F);
	cv::minMaxLoc(cv::abs(ks), &minVal, &maxVal); //normalize Sobel kernel to the same as Canny would normally expect
	//also normalize by color space
	
	sfg_mag /= (colorspaceApproxMaxx*((float)maxVal));
	
	// based on |magnitude|^2
	
	cv::multiply(sfg_x, sfg_mag, returned_dX);
	cv::multiply(sfg_y, sfg_mag, returned_dY);
}
//-----------------------------------------------------------------------------------------

void cppCannyBunk_Internal( const cv::Mat& image, cv::Mat& edges,
                double threshold1, double threshold2,
                int apertureSize )
{
	cv::Mat src = image;
	std::vector<cv::Mat> channels;
	cv::split(src, channels);
	float numChannels = 0.0f;
	int kk;
	
	cv::Mat totalEdgeX(src.size(), CV_32F, cv::Scalar(0.0f));
	cv::Mat totalEdgeY(src.size(), CV_32F, cv::Scalar(0.0f));
	cv::Mat thisEdgedX, thisEdgedY;
	

	for(kk=0; kk<channels.size(); kk++) {
		if(channels[kk].type() != CV_32F) {
			channels[kk].convertTo(channels[kk], CV_32F);
		}
		numChannels += 1.0f;
		edge_get_dXdY_with_angles_constrained_0_to_180_deg(channels[kk], apertureSize, thisEdgedX, thisEdgedY);
		
		if(kk == 0) {
			totalEdgeX += thisEdgedX;
			totalEdgeY += thisEdgedY;
		} else {
			cv::Mat ang1,ang2;
			cv::phase(totalEdgeX, totalEdgeY, ang1, true);
			cv::phase(thisEdgedX, thisEdgedY, ang2, true);
			
			float adiff;
			for(int ii=0; ii<src.rows; ii++) {
				for(int jj=0; jj<src.cols; jj++) {
					adiff = abs(ang1.at<float>(ii,jj) - ang2.at<float>(ii,jj));
					adiff = std::min(360.0f-adiff, adiff);
					if(adiff <= 90.0f) {
						totalEdgeX.at<float>(ii,jj) += thisEdgedX.at<float>(ii,jj);
						totalEdgeY.at<float>(ii,jj) += thisEdgedY.at<float>(ii,jj);
					} else {
						totalEdgeX.at<float>(ii,jj) -= thisEdgedX.at<float>(ii,jj);
						totalEdgeY.at<float>(ii,jj) -= thisEdgedY.at<float>(ii,jj);
					}
				}
			}
		}
	}
	totalEdgeX /= numChannels;
	totalEdgeY /= numChannels;
	
	totalEdgeX.convertTo(totalEdgeX, CV_16S);
	totalEdgeY.convertTo(totalEdgeY, CV_16S);
	
	cppCannyNearlyOriginalInternal(totalEdgeX, totalEdgeY, edges, threshold1, threshold2);
}

void cppCannyBunk_RGB( const cv::Mat& image, cv::Mat& edges,
                double threshold1, double threshold2,
                int apertureSize )
{
	colorspaceApproxMaxx = 255.0f;
	cppCannyBunk_Internal(image, edges, threshold1, threshold2, apertureSize);
}

void cppCannyBunk_CIELAB( const cv::Mat& image, cv::Mat& edges,
                double threshold1, double threshold2,
                int apertureSize, double colorspacescalar /* = 1.0 */ )
{
	CV_Assert(image.channels() == 3);
	CV_Assert(image.type() == CV_8UC3 || image.type() == CV_32FC3);
	
	cv::Mat srcConverted;
	if(image.type() != CV_32FC3) {
		image.convertTo(srcConverted, CV_32FC3);
		srcConverted = (srcConverted / 255.0f);
	} else {
		srcConverted = image.clone();
	}
	cv::cvtColor(srcConverted, srcConverted, CV_BGR2Lab);
	
	colorspaceApproxMaxx = 95.0f * ((float)colorspacescalar);
	
	cppCannyBunk_Internal(srcConverted, edges, threshold1, threshold2, apertureSize);
}

/* End of file. */
