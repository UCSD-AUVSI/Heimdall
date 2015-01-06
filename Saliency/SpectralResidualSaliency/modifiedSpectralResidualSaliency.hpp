/*M///////////////////////////////////////////////////////////////////////////////////////
 //
 //  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 //
 //  By downloading, copying, installing or using the software you agree to this license.
 //  If you do not agree to this license, do not download, install,
 //  copy or use the software.
 //
 //
 //                           License Agreement
 //                For Open Source Computer Vision Library
 //
 // Copyright (C) 2014, OpenCV Foundation, all rights reserved.
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
 //   * The name of the copyright holders may not be used to endorse or promote products
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

#ifndef __MODIFIED_OPENCV_SALIENCY_SPECIALIZED_CLASSES_HPP__
#define __MODIFIED_OPENCV_SALIENCY_SPECIALIZED_CLASSES_HPP__

//#include "opencv2/saliency/kyheader.hpp"
#include <cstdio>
#include <string>
#include <iostream>
#include <stdint.h>
#include <opencv2/opencv.hpp>


//! @addtogroup saliency
//! @{

/************************************ Specific Static Saliency Specialized Classes ************************************/

/** @brief the Spectral Residual approach from  @cite SR

Starting from the principle of natural image statistics, this method simulate the behavior of
pre-attentive visual search. The algorithm analyze the log spectrum of each image and obtain the
spectral residual. Then transform the spectral residual to spatial domain to obtain the saliency
map, which suggests the positions of proto-objects.
 */
class modified_StaticSaliencySpectralResidual
{
	void GetWindow(cv::Mat & win);
public:

  bool computeSaliency( const cv::Mat image, cv::OutputArray saliencyMap,
					cv::Mat * processedImgGray = NULL );
					
  bool computeSaliencyImpl_multiChannel(const cv::Mat image, cv::OutputArray saliencyMap);

};


#endif
