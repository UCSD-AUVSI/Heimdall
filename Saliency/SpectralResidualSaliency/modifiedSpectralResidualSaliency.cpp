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

#include "modifiedSpectralResidualSaliency.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <iostream>
using std::cout; using std::endl;
#ifndef CV_BGR2GRAY
#define CV_BGR2GRAY cv::COLOR_BGR2GRAY
#endif
#include <thread>


/**
 * SaliencySpectralResidual
 */


bool modified_StaticSaliencySpectralResidual::computeSaliency( const cv::Mat image, cv::OutputArray saliencyMap,
																cv::Mat * processedImgGray /* = NULL */ )
{
  cv::Mat grayTemp;
  std::vector<cv::Mat> mv;
  cv::Size givenImageSize(image.size());
  
  
  cv::Mat realImage( givenImageSize, CV_64F );
  cv::Mat imaginaryImage( givenImageSize, CV_64F );
  imaginaryImage.setTo( 0 );
  cv::Mat combinedImage( givenImageSize, CV_64FC2 );
  cv::Mat imageDFT;
  cv::Mat logAmplitude;
  cv::Mat angle( givenImageSize, CV_64F );
  cv::Mat magnitude( givenImageSize, CV_64F );
  cv::Mat logAmplitude_blur;

  if( image.channels() == 3 )
  {
    cv::cvtColor( image, grayTemp, CV_BGR2GRAY );
    grayTemp.convertTo( realImage, CV_64F );
  }
  else
  {
    image.convertTo( realImage, CV_64F );
  }

  mv.push_back( realImage );
  mv.push_back( imaginaryImage );
  merge( mv, combinedImage );
  cv::dft( combinedImage, imageDFT );
  cv::split( imageDFT, mv );

  //-- Get magnitude and phase of frequency spectrum --//
  cv::cartToPolar( mv.at( 0 ), mv.at( 1 ), magnitude, angle, false );
  cv::log( magnitude, logAmplitude );
  //-- Blur log amplitude with averaging filter --//
  cv::blur( logAmplitude, logAmplitude_blur, cv::Size( 3, 3 ), cv::Point( -1, -1 ), cv::BORDER_REPLICATE );

  cv::exp( logAmplitude - logAmplitude_blur, magnitude );
  //-- Back to cartesian frequency domain --//
  cv::polarToCart( magnitude, angle, mv.at( 0 ), mv.at( 1 ), false );
  cv::merge( mv, imageDFT );
  cv::dft( imageDFT, combinedImage, cv::DFT_INVERSE );
  cv::split( combinedImage, mv );

  cv::cartToPolar( mv.at( 0 ), mv.at( 1 ), magnitude, angle, false );
  cv::GaussianBlur( magnitude, magnitude, cv::Size( 5, 5 ), 8, 0, cv::BORDER_REPLICATE );
  magnitude = magnitude.mul( magnitude );

  double minVal, maxVal;
  cv::minMaxLoc( magnitude, &minVal, &maxVal );
  cout<<"spectral magnitude maxVal: "<<maxVal<<endl;

  magnitude = magnitude / maxVal;
  
  magnitude.convertTo( saliencyMap, CV_32F );
  /*magnitude.convertTo( magnitude, CV_32F );
  resize( magnitude, saliencyMap, image.size(), 0, 0, INTER_LINEAR );*/
	
#if 1
	cv::Mat windowfunc = cv::Mat::ones(saliencyMap.size(), CV_32F);
	GetWindow(windowfunc);
	
	cv::multiply(saliencyMap, windowfunc, saliencyMap);
#endif

  return true;
}


//returns saliency map for one channel of a multi-channel image
//
void single_channel_spectsal_compute(cv::Mat* singleChannelImg, cv::Mat* returnedSum)
{
	cv::Size givenImageSize(singleChannelImg->size());
	
	cv::Mat logAmplitude;
	singleChannelImg->convertTo( logAmplitude, CV_64F ); //save real part in "logAmplitude" for now
	cv::Mat combinedImage( givenImageSize, CV_64FC2 );
	cv::Mat imageDFT;
	cv::Mat angle( givenImageSize, CV_64F );
	cv::Mat magnitude( givenImageSize, CV_64F );
	cv::Mat logAmplitude_blur;
	
	std::vector<cv::Mat> mv;
	mv.push_back( logAmplitude ); //real part for DFT
	mv.push_back( cv::Mat::zeros(givenImageSize, CV_64F) ); //imaginary part for DFT
	merge( mv, combinedImage );
	cv::dft( combinedImage, imageDFT );
	cv::split( imageDFT, mv );

	//-- Get magnitude and phase of frequency spectrum --//
	cv::cartToPolar( mv.at( 0 ), mv.at( 1 ), magnitude, angle, false );
	cv::log( magnitude, logAmplitude );
	//-- Blur log amplitude with averaging filter --//
	cv::blur( logAmplitude, logAmplitude_blur, cv::Size( 3, 3 ), cv::Point( -1, -1 ), cv::BORDER_REFLECT_101 );
	
	cv::exp( logAmplitude - logAmplitude_blur, magnitude );
	//-- Back to cartesian frequency domain --//
	cv::polarToCart( magnitude, angle, mv.at( 0 ), mv.at( 1 ), false );
	cv::merge( mv, imageDFT );
	cv::dft( imageDFT, combinedImage, cv::DFT_INVERSE );
	cv::split( combinedImage, mv );
	
	cv::cartToPolar( mv.at( 0 ), mv.at( 1 ), magnitude, angle, false );
	cv::GaussianBlur( magnitude, magnitude, cv::Size( 5, 5 ), 8, 0, cv::BORDER_REFLECT_101 );
	
	(*returnedSum) = magnitude.mul( magnitude );  //return saliency map ^2 so that several a 3-channel image can be done using sqrt( ch0^2 + ch1^2 + ch2^2 )
}


bool modified_StaticSaliencySpectralResidual::computeSaliencyImpl_multiChannel(const cv::Mat image, cv::OutputArray saliencyMap)
{
	int ii;
	cv::Size givenImageSize(image.size());
	std::vector<cv::Mat> imgChannels;
	cv::split(image, imgChannels);
	cv::Mat magSum(givenImageSize, CV_64F, cv::Scalar(0));
	
	std::vector<cv::Mat> resultsSavedHere(3);
	std::vector<std::thread*> threadslaunched;
	for(ii=0; ii<imgChannels.size(); ii++) {
        //cout<<"launching saliency thread "<<ii<<" of 3"<<endl;
		threadslaunched.push_back(new std::thread(single_channel_spectsal_compute, &(imgChannels[ii]), &(resultsSavedHere[ii])));
	}
	
	for(ii=0; ii<imgChannels.size(); ii++) {
		threadslaunched[ii]->join();
		delete threadslaunched[ii]; threadslaunched[ii] = nullptr;
		magSum += resultsSavedHere[ii];
	}
	
	magSum.convertTo(magSum, CV_32F);
	
#if 1
	cv::Mat windowfunc = cv::Mat::ones(image.size(), CV_32F);
	GetWindow(windowfunc);
	cv::multiply(magSum, windowfunc, magSum);
#endif
	
	magSum.copyTo(saliencyMap);

	return true;
}



void modified_StaticSaliencySpectralResidual::GetWindow(cv::Mat & win)
{
	for(int jj=0; jj<win.cols; jj++) {
		win.at<float>(0,jj) = 0.0f;
		win.at<float>(1,jj) = 0.33333333333333333f;
		win.at<float>(2,jj) = 0.66666666666666667f;
		win.at<float>(win.rows-1,jj) = 0.0f;
		win.at<float>(win.rows-2,jj) = 0.33333333333333333f;
		win.at<float>(win.rows-3,jj) = 0.66666666666666667f;
	}
	int rmax2 = (win.rows-1);
	for(int ii=1; ii<rmax2; ii++) {
		win.at<float>(ii,0) = 0.0f;
		win.at<float>(ii,1) = 0.33333333333333333f;
		win.at<float>(ii,2) = 0.66666666666666667f;
		win.at<float>(ii,win.cols-1) = 0.0f;
		win.at<float>(ii,win.cols-2) = 0.33333333333333333f;
		win.at<float>(ii,win.cols-3) = 0.66666666666666667f;
	}
	win.at<float>(1,2) = 0.33333333333333333f;
	win.at<float>(1,win.cols-3) = 0.33333333333333333f;
	
	win.at<float>(win.rows-2,2) = 0.33333333333333333f;
	win.at<float>(win.rows-2,win.cols-3) = 0.33333333333333333f;
}



