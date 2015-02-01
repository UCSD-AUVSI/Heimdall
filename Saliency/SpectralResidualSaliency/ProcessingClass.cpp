#include "ProcessingClass.hpp"
#include "modifiedSpectralResidualSaliency.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/connected_components.h"
#include <iostream>
#include <map>
using std::cout; using std::endl;


static void low_thresh_hysteresis_for_blobs(cv::Mat & floatSaliencyMap,
											cv::Mat & highThreshedMap,
											const SpectralSaliencyArgs & args)
{
	int ii,jj, ik,jk, i2,j2;
	if(args.centerSurroundThreshold_hystlow_FRACTION > 0.0) {
		
		const int kpad = 2;
		const int rowMax = (highThreshedMap.rows-kpad-1);
		const int colMax = (highThreshedMap.cols-kpad-1);
		//cout<<"doing hysteresis size thresholding"<<endl;
		//cv::minMaxLoc(gblur_small, &minVal, &maxVal);
		//cout<<"      -- gblur_small -- max == "<<maxVal<<endl;
		
		const int num_iterations = 5;
		float thisThresh;
		
		//###################################################################################
		//cv::imwrite("../../output_images/binary_before_hyst.png", highThreshedMap);
		//###################################################################################
#if 1
		// using a bool.
		cv::Mat connectionLabelsOut = cv::Mat::zeros(highThreshedMap.size(), CV_8U);
		ConnectedComponents cc(255);
		cc.connected(highThreshedMap.data, connectionLabelsOut.data, connectionLabelsOut.cols, connectionLabelsOut.rows,
			std::equal_to<unsigned char>(),
			true);
		
		std::map<unsigned char, float> blob_threshes;
		
		unsigned char thisLabel;
		float thisFloatAt;
		for(ii=0; ii<highThreshedMap.rows; ii++) {
			for(jj=0; jj<highThreshedMap.cols; jj++) {
				if(highThreshedMap.at<unsigned char>(ii,jj) != 0) {
					thisLabel = connectionLabelsOut.at<unsigned char>(ii,jj);
					thisFloatAt = floatSaliencyMap.at<float>(ii,jj);
					if(blob_threshes.find(thisLabel) == blob_threshes.end()) {
						blob_threshes[thisLabel] = thisFloatAt;
					} else {
						blob_threshes[thisLabel] = MAX(thisFloatAt, blob_threshes[thisLabel]);
					}
				}
			}
		}
		for(auto mapiter = blob_threshes.begin(); mapiter != blob_threshes.end(); mapiter++) {
			mapiter->second *= args.centerSurroundThreshold_hystlow_FRACTION;
			cout<<"threshold for blob "<<to_istring(mapiter->first)<<" is "<<(mapiter->second)<<endl;
		}
		
		//###################################################################################
		//cv::imwrite("../../output_images/connections.png", connectionLabelsOut);
		//###################################################################################
		
		//---------------------------------
		// Use a relative low-threshold
		//---------------------------------
		unsigned char thisThreshBlobLabel;
		unsigned char otherLabel;
		float otherThresh;
		for(int it=0; it<num_iterations; it++) {
			for(ii=kpad; ii<=rowMax; ii++) {
				for(jj=kpad; jj<=colMax; jj++) {
					if(highThreshedMap.at<unsigned char>(ii,jj) != 0) {
						
						thisThreshBlobLabel = connectionLabelsOut.at<unsigned char>(ii,jj);
						thisThresh = blob_threshes[thisThreshBlobLabel];
						
						for(ik=-kpad; ik<=kpad; ik++) {
							for(int jk=-kpad; jk<=kpad; jk++) {
								i2=(ii+ik);
								j2=(jj+jk);
								otherLabel = connectionLabelsOut.at<unsigned char>(i2,j2);
								if(otherLabel != thisThreshBlobLabel) { //if pixel isn't currently part of this blob
									if(floatSaliencyMap.at<float>(i2,j2) > thisThresh) { //if salient intensity is higher than this blob's calculated threshold
										highThreshedMap.at<unsigned char>(i2,j2) = 255; //make sure pixel isn't background
										connectionLabelsOut.at<unsigned char>(i2,j2) = thisThreshBlobLabel;
										
										if(otherLabel != 0 && otherLabel != thisThreshBlobLabel) {
											otherThresh = blob_threshes[otherLabel];
											blob_threshes[otherLabel] = blob_threshes[thisThreshBlobLabel] = MIN(thisThresh, otherThresh);
										}
									}
								}
							}
						}
					}
				}
			}
		}
#else
		//---------------------------------
		// Use an absolute low-threshold
		//---------------------------------
		thisThresh = args.centerSurroundThreshold_hystlow_FRACTION;
		
		for(int it=0; it<num_iterations; it++) {
			for(ii=kpad; ii<=rowMax; ii++) {
				for(jj=kpad; jj<=colMax; jj++) {
					if(highThreshedMap.at<unsigned char>(ii,jj) != 0) {
						for(ik=-kpad; ik<=kpad; ik++) {
							for(int jk=-kpad; jk<=kpad; jk++) {
								//if(ik != jk || ik != 0) { //don't bother checking the center, we know it's good
									if(floatSaliencyMap.at<float>(ii+ik,jj+jk) > thisThresh) {
										highThreshedMap.at<unsigned char>(ii+ik,jj+jk) = 255;
									}
								//}
							}
						}
					}
				}
			}
		}
#endif
		//###################################################################################
		//cv::imwrite("../../output_images/binary_after_hyst.png", highThreshedMap);
		//###################################################################################
	}
}


static void binarize_static_saliency_image(cv::Mat & rawSaliencyMap,
										cv::Mat* centerSurroundIntermediate,
										const SpectralSaliencyArgs & args)
{
	int gblur_radius = args.largerGaussBlurDiamPixels;
	if(gblur_radius % 2 == 0) {
		gblur_radius--;
	}
	if(gblur_radius < 5) {
		gblur_radius = 5;
	}
	
	cv::Mat gblur_small, gblur_large;
	cv::GaussianBlur(rawSaliencyMap, gblur_small, cv::Size(3,3), 0.0, 0.0, cv::BORDER_REPLICATE);
	cv::GaussianBlur(rawSaliencyMap, gblur_large, cv::Size(gblur_radius,gblur_radius), 0.0, 0.0, cv::BORDER_REPLICATE);
	
	gblur_small -= gblur_large;
	
	//--------------------------------------------
	double minVal,maxVal;
	cv::minMaxLoc(gblur_small, &minVal, &maxVal);
	cout<<"SpectralSaliency -- gblur_small -- max == "<<maxVal<<endl;
	if(centerSurroundIntermediate != nullptr) {
		gblur_small.copyTo(*centerSurroundIntermediate);
	}
	//--------------------------------------------
	
    cv::threshold(gblur_small, rawSaliencyMap, args.centerSurroundThreshold, 255.0, cv::THRESH_BINARY);
    rawSaliencyMap.convertTo(rawSaliencyMap, CV_8U);
    
    low_thresh_hysteresis_for_blobs(gblur_small, rawSaliencyMap, args);
}


void SpectralResidualSaliencyClass::ProcessSaliency(cv::Mat * fullsizeImage,
												std::vector<cv::Mat> * returnedCrops,
												std::vector<std::pair<double,double>> * returned_geolocations,
												int threadNumForDebugging)
{
	returnedCrops->clear();
	returned_geolocations->clear();
	double minVal,maxVal;
	std::vector<double> resImRatio;
	modified_StaticSaliencySpectralResidual mssaliency;
	cv::Mat binaryMap;
	cv::Mat saliencyMap;
	cv::Mat totalSaliencyMapTwo;
	
	for(int loopsii=0; loopsii<2; loopsii++) {
		resImRatio.push_back(0.0);
		if(loopsii == 0) {
			resImRatio[loopsii] = (9.0 / args.expectedTargetLength);
		} else {
			resImRatio[loopsii] = (9.0 / (args.expectedTargetLength * args.expectedLargerTargetRatio));
		}
		//cout << "SpectralSaliency -- resImRatio == "<<resImRatio<<endl;
		double maxcols = ((double)fullsizeImage->cols) * resImRatio[loopsii];
		double maxrows = ((double)fullsizeImage->rows) * resImRatio[loopsii];
		//cout<<"maxcols, maxrows == "<<maxcols<<", "<<maxrows<<endl;
		//assert(maxcols < 800.0 && maxrows < 800.0);
		
		/*
			Converting to CIELab is slow, so resize FIRST
		*/
		cv::Mat fullsizeImage_CIELAB;
		cv::resize(*fullsizeImage, fullsizeImage_CIELAB, cv::Size(0,0), resImRatio[loopsii], resImRatio[loopsii], cv::INTER_AREA);
		fullsizeImage_CIELAB.convertTo(fullsizeImage_CIELAB, CV_32FC3);
		fullsizeImage_CIELAB /= 255.0f;
		
		cv::cvtColor(fullsizeImage_CIELAB, fullsizeImage_CIELAB, CV_BGR2Lab);
		
		
		//if(check_if_directory_exists(args.save_output_to_this_folder))
		//	cv::imwrite(args.save_output_to_this_folder+std::string("/resized_size")+to_istring(loopsii)+std::string(".png"), fullsizeImage_CIELAB*2.5f);
		
		
		mssaliency.computeSaliencyImpl_multiChannel(fullsizeImage_CIELAB, saliencyMap);
		if(args.normalizeSaliencyMaps) {
			cv::minMaxLoc(saliencyMap, &minVal, &maxVal);
			saliencyMap /= maxVal;
		}
		
		
		if(saveIntermediateResults) {
			saliencyMap.copyTo(last_saliency_map);
			cv::minMaxLoc(last_saliency_map, &minVal, &maxVal);
			last_saliency_map /= maxVal;
		}
		
		
		if(check_if_directory_exists(args.save_output_to_this_folder))
			cv::imwrite(args.save_output_to_this_folder+std::string("/0salmap_size")+to_istring(loopsii)+std::string(".png"), saliencyMap*255.0f);
		
		
		if(loopsii == 0) {
			saliencyMap.copyTo(totalSaliencyMapTwo);
			continue;
		}
		else {
			if(saliencyMap.cols > totalSaliencyMapTwo.cols || saliencyMap.rows > totalSaliencyMapTwo.rows) {
				cout<<"########################### WARNING: SECOND-PASS SALIENCY MAP WAS LARGER!!!! BUT IT SHOULD BE SMALLER TO LOOK FOR BIGGER OBJECTS"<<endl;
			}
			if(saliencyMap.size() != totalSaliencyMapTwo.size()) {
				cv::resize(saliencyMap, saliencyMap, totalSaliencyMapTwo.size(), 0.0, 0.0, cv::INTER_LINEAR);
			}
			saliencyMap += totalSaliencyMapTwo;
			saliencyMap *= 0.5f;
		}
		
		
		if(check_if_directory_exists(args.save_output_to_this_folder))
			cv::imwrite(args.save_output_to_this_folder+std::string("/0salmap_total.png"), saliencyMap*255.0f);
		
		
		if(saveIntermediateResults) {
			binarize_static_saliency_image(saliencyMap, &last_centersurround_map, args);
			saliencyMap.copyTo(last_binary_map);
		} else {
			binarize_static_saliency_image(saliencyMap, nullptr, args);
		}
		
		
		if(saveIntermediateResults && check_if_directory_exists(args.save_output_to_this_folder)) {
			cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_total.png"), last_centersurround_map*255.0f);
		}
		/*if(saveIntermediateResults && check_if_directory_exists(args.save_output_to_this_folder)) {
			if(loopsii == 0) {
				cout<<"@@@@@@@@@@@@@@@@@@@@@ center surround map type: "<<to_istring(last_centersurround_map.type())<<endl;
				cout<<"CV_8U == "<<to_istring(CV_8U)<<", CV_32F == "<<to_istring(CV_32F)<<endl;
				last_centersurround_map.copyTo(totalSaliencyMapTwo);
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size0.png"), totalSaliencyMapTwo*255.0f);
				cv::multiply(totalSaliencyMapTwo, totalSaliencyMapTwo, totalSaliencyMapTwo);
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size0_sqrd.png"), totalSaliencyMapTwo*255.0f);
				cv::Mat copyOFMAPTWO;
				cv::sqrt(totalSaliencyMapTwo, copyOFMAPTWO);
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size0_sqrd_sqrt.png"), copyOFMAPTWO*255.0f);
			} else {
				//this one should be smaller, so scale up
				cv::Mat upscaledCS;
				last_centersurround_map.copyTo(upscaledCS);
				if(upscaledCS.size() != totalSaliencyMapTwo.size()) {
					cv::resize(upscaledCS, upscaledCS, totalSaliencyMapTwo.size(), 0.0, 0.0, cv::INTER_LINEAR);
				}
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size1.png"), upscaledCS*255.0f);
				
				cv::multiply(upscaledCS, upscaledCS, upscaledCS);
				
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size1_sqrd.png"), upscaledCS*255.0f);
				
				
				totalSaliencyMapTwo += upscaledCS;
				
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_total_beforesqrt.png"), totalSaliencyMapTwo*255.0f);
				
				cv::sqrt(totalSaliencyMapTwo, totalSaliencyMapTwo);
				totalSaliencyMapTwo /= sqrt(2.0f);
				
				cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_total.png"), totalSaliencyMapTwo*255.0f);
				//cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size1_notresized.png"), last_centersurround_map*255.0f);
			}
			//cv::imwrite(args.save_output_to_this_folder+std::string("/1centersurround_size")+to_istring(loopsii)+std::string(".png"), last_centersurround_map*255.0f);
		}
		//if(check_if_directory_exists(args.save_output_to_this_folder))
		//	cv::imwrite(args.save_output_to_this_folder+std::string("/thresh_size")+to_istring(loopsii)+std::string(".png"), saliencyMap);
		*/
		
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(saliencyMap, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		
		for(int ii=0; ii<contours.size(); ii++) {
			cv::Rect bounds = cv::boundingRect(contours[ii]);
			
			double b_x = (   ((double)bounds.x) / resImRatio[0]   );
			double b_y = (   ((double)bounds.y) / resImRatio[0]   );
			double b_wid = (   ((double)(bounds.width)) / resImRatio[0]   );
			double b_hei = (   ((double)(bounds.height)) / resImRatio[0]   );
			
			double b_length = sqrt(b_wid*b_hei);
			
			cout<<"potential target "<<to_istring(ii)<<" has length scale "<<b_length<<endl;
			
			if(b_length >= args.minTargetLength && b_length <= args.maxTargetLength)
			{
				double padPixelsH = ((b_wid*0.5)*((1.0/args.percentOfCropToBeTargetAfterPadding)-1.0));
				double padPixelsV = ((b_hei*0.5)*((1.0/args.percentOfCropToBeTargetAfterPadding)-1.0));
				
				b_x -= padPixelsH;
				b_wid += (2.0*padPixelsH);
				b_y -= padPixelsV;
				b_hei += (2.0*padPixelsV);
				
				bounds.x = RoundDoubleToInt(b_x);
				bounds.x = MAX(0, bounds.x);
				
				bounds.y = RoundDoubleToInt(b_y);
				bounds.y = MAX(0, bounds.y);
				
				bounds.width = RoundDoubleToInt(b_wid);
				if((bounds.x+bounds.width) > fullsizeImage->cols) {
					bounds.width = (fullsizeImage->cols - bounds.x);
				}
				
				bounds.height = RoundDoubleToInt(b_hei);
				if((bounds.y+bounds.height) > fullsizeImage->rows) {
					bounds.height = (fullsizeImage->rows - bounds.y);
				}
				
				returnedCrops->push_back(cv::Mat(*fullsizeImage, bounds));
				returned_geolocations->push_back(std::pair<double,double>(bounds.x,bounds.y));
			}
		}
	}
}



void SpectralSaliencyArgs::GetVec(std::vector<double> & putHere)
{
	putHere.clear();
	putHere.push_back(expectedTargetLength);
	putHere.push_back(expectedLargerTargetRatio);
	putHere.push_back(minTargetLength);
	putHere.push_back(maxTargetLength);
	putHere.push_back((double)largerGaussBlurDiamPixels);
	putHere.push_back(centerSurroundThreshold);
	putHere.push_back(centerSurroundThreshold_hystlow_FRACTION);
	putHere.push_back(percentOfCropToBeTargetAfterPadding);
}

/*static*/ SpectralSaliencyArgs SpectralSaliencyArgs::GetArgs(const std::vector<double> & argsvec)
{
	assert(argsvec.size() == 8);
	
	SpectralSaliencyArgs returned;
	returned.expectedTargetLength = argsvec[0];
	returned.expectedLargerTargetRatio = argsvec[1];
	returned.minTargetLength = argsvec[2];
	returned.maxTargetLength = argsvec[3];
	returned.largerGaussBlurDiamPixels = ((int)argsvec[4]);
	returned.centerSurroundThreshold = argsvec[5];
	returned.centerSurroundThreshold_hystlow_FRACTION = argsvec[6];
	returned.percentOfCropToBeTargetAfterPadding = argsvec[7];
	return returned;
}

