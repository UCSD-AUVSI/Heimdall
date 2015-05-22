#include <iostream>
using std::cout;
using std::endl;
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "Saliency/SpectralResidualSaliency/ProcessingClass.hpp"
#include <chrono>

const std::string folderOutputPath("../../output_images");


int main(int argc, char** argv)
{
	cout << "Test Spectral Residual Saliency" << endl;
	if(argc < 2) {
		//cout << "usage:  [PATH-TO-IMAGE]" << endl;
		cout << "usage:  [PATH-TO-FOLDER-WITH-IMAGES]" << endl;
		return 1;
	}
	std::string imageFolderPath(argv[1]);
	while(imageFolderPath[imageFolderPath.size()-1] == '/') {
		imageFolderPath.erase(imageFolderPath.size()-1);
	}
	if(check_if_directory_exists(imageFolderPath) == false) {
		cout<<"error: directory \""<<imageFolderPath<<"\" invalid!"<<endl;
	}
	std::vector<std::string> imageFilenames = GetImageFilenamesInFolder(argv[1]);
	if(imageFilenames.empty()) {
		cout<<"warning: no images found in that directory!!"<<endl;
	}
	
	std::vector<double> timings;
	
	for(int jj=0; jj<imageFilenames.size(); jj++)
	{
		cv::Mat fullsizeImage = cv::imread(imageFolderPath+std::string("/")+imageFilenames[jj], CV_LOAD_IMAGE_COLOR);
		
		//------------------------------------------------------
		std::string imgName(imageFilenames[jj]);
		trim_chars_after_delim(imgName, '.', false);
		cout<<"IMAGE: \""<<imgName<<"\""<<endl;
		//------------------------------------------------------
		
		std::vector<cv::Mat> foundCrops;
		std::vector<std::pair<double,double>> cropGeolocations;
		
		SpectralResidualSaliencyClass saldoer;
		//saldoer.args.save_output_to_this_folder = folderOutputPath;
		//saldoer.saveIntermediateResults = true;
		
		auto tstart = std::chrono::steady_clock::now();
		saldoer.ProcessSaliency(&fullsizeImage, &foundCrops, &cropGeolocations, 0);
		auto tend = std::chrono::steady_clock::now();
		timings.push_back(std::chrono::duration<double,std::milli>(tend-tstart).count());
		std::cout<<"that test took "<<timings.back()<<" milliseconds"<<std::endl;
		
		//cv::waitKey(0);
		
		cv::Mat bw_salmap;
		saldoer.last_saliency_map.copyTo(bw_salmap);
		bw_salmap *= 255.0f;
		bw_salmap.convertTo(bw_salmap, CV_8U);
		std::vector<cv::Mat> threeChannels;
		threeChannels.resize(3, bw_salmap);
		cv::merge(threeChannels, bw_salmap);
		
		cv::Mat bw_csmap;
		saldoer.last_centersurround_map.copyTo(bw_csmap);
		bw_csmap *= 255.0f;
		bw_csmap.convertTo(bw_csmap, CV_8U);
		threeChannels.clear();
		threeChannels.resize(3, bw_csmap);
		cv::merge(threeChannels, bw_csmap);
		
		cv::Mat bw_binmap;
		saldoer.last_binary_map.copyTo(bw_binmap);
		threeChannels.clear();
		threeChannels.resize(3, bw_binmap);
		cv::merge(threeChannels, bw_binmap);
		
		
		if(foundCrops.empty() == false) {
			for(int ii=0; ii<foundCrops.size(); ii++) {
				std::string cropname = std::string("crop")+to_istring(ii);
				std::cout<<"crop"<<to_istring(ii)<<" found at geo-location: "<<cropGeolocations[ii].first<<","<<cropGeolocations[ii].second<<std::endl;
				
				double ratioX = ((double)bw_salmap.cols) / ((double)fullsizeImage.cols);
				double ratioY = ((double)bw_salmap.rows) / ((double)fullsizeImage.rows);
				
				cout<<"ratio of sal map to fullsize: ("<<ratioX<<","<<ratioY<<")"<<endl;
				
				cv::Rect cropRect(	RoundDoubleToInt(((double)cropGeolocations[ii].first)*ratioX),
									RoundDoubleToInt(((double)cropGeolocations[ii].second)*ratioY),
									RoundDoubleToInt(((double)foundCrops[ii].cols)*ratioX),
									RoundDoubleToInt(((double)foundCrops[ii].rows)*ratioY));
				
				cout << "crop"<<to_istring(ii)<<" rectangle: "<<cropRect<<endl;
				
				//cv::imshow(cropname, foundCrops[ii]);
				cv::imwrite(folderOutputPath+std::string("/")+imgName+std::string("_crop")+to_istring(ii)+std::string(".jpg"), foundCrops[ii]);
				
				cv::rectangle(bw_salmap, cropRect, cv::Scalar(0,255,255), 1, 4);
				cv::rectangle(bw_csmap,  cropRect, cv::Scalar(0,255,255), 1, 4);
				cv::rectangle(bw_binmap,  cropRect, cv::Scalar(0,255,255), 1, 4);
			}
			//cv::waitKey(0);
		}
		else {
			std::cout << "no crops found in this image!" << std::endl;
		}
		
		//cv::imwrite(folderOutputPath+std::string("/")+imgName+std::string("_0_salmap.png"), bw_salmap);
		//cv::imwrite(folderOutputPath+std::string("/")+imgName+std::string("_1_centersurround.png"), bw_csmap);
		//cv::imwrite(folderOutputPath+std::string("/")+imgName+std::string("_binary_result.png"), bw_binmap);
	}
	
	double averagetime = 0.0;
	for(int ii=0; ii<timings.size(); ii++) {
		averagetime += timings[ii];
	}
	averagetime /= ((double)(timings.size()));
	std::cout<<std::endl<<"average time saliency took to process an image: "<<averagetime<<" milliseconds"<<std::endl<<std::endl;
	
	return 0;
}

