#include "experimentUtils.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using std::cout; using std::endl;
#include "SharedUtils/SharedUtils_RNG.hpp"


void loadImagesUncompressedMemory(std::string folder, int max_number_of_images_to_load,
                                std::vector<cv::Mat*> & returnedImages,
                                std::vector<std::string> & returned_image_filenames)
{
	assert(returnedImages.empty());
	returned_image_filenames.clear();
	
	if(max_number_of_images_to_load <= 0) {
		return;
	}
	
	std::vector<std::string> tImgPaths;
	std::vector<std::string> tImgFnames;
	
	int compressedSizeOfAllImages = 0;
	int numFoundImages = 0;

	tinydir_dir dir;
	tinydir_open(&dir, folder.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.' && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			tImgFnames.push_back(file.name);
			tImgPaths.push_back(file.path);
			numFoundImages++;
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	
	if(numFoundImages <= max_number_of_images_to_load) {
		for(int ii=0; ii<numFoundImages; ii++) {
			returned_image_filenames.push_back(tImgFnames[ii]);
			returnedImages.push_back(new cv::Mat(cv::imread(tImgPaths[ii],CV_LOAD_IMAGE_COLOR)));
			compressedSizeOfAllImages += (returnedImages.back()->rows * returnedImages.back()->cols);
			cout<<"found "<<returnedImages.size()<<" images so far, total size: "<<static_cast<int>(((double)compressedSizeOfAllImages)*0.001*0.001)<<" megabytes"<<endl;
		}
	}
	else {
		while(returnedImages.size() != max_number_of_images_to_load)
		{
			RNG_rand_r rngg;
			int newidx = rngg.rand_int(0, ((int)tImgPaths.size()) - 1);
			
			returnedImages.push_back(new cv::Mat(cv::imread(tImgPaths[newidx],CV_LOAD_IMAGE_COLOR)));
			compressedSizeOfAllImages += (returnedImages.back()->rows * returnedImages.back()->cols);
			returned_image_filenames.push_back(tImgFnames[newidx]);
			tImgPaths.erase( tImgPaths.begin() + newidx);
			tImgFnames.erase(tImgFnames.begin() + newidx);
			
			cout<<"chose image \""<<returned_image_filenames.back()<<"\" for study, total size so far: "<<static_cast<int>(((double)compressedSizeOfAllImages)*0.001*0.001)<<" megabytes"<<endl;
		}
	}
	assert(returnedImages.size() == returned_image_filenames.size());
}


void loadImagesIntoCompressedMemory(std::string folder, int max_number_of_images_to_load,
						std::vector<std::vector<unsigned char>*> & returned_png_images,
						std::vector<std::string> & returned_image_filenames)
{
	assert(returned_png_images.empty());
	returned_image_filenames.clear();
	
	if(max_number_of_images_to_load <= 0) {
		return;
	}
	
	std::vector<std::string> tImgPaths;
	std::vector<std::string> tImgFnames;
	
	std::vector<unsigned char> *newarr_writehere;
	std::vector<int> pngparam = std::vector<int>(2);
	pngparam[0] = CV_IMWRITE_PNG_COMPRESSION; pngparam[1] = 3; //0-9... 9 is smallest compressed size
	int compressedSizeOfAllImages = 0;
	int numFoundImages = 0;

	tinydir_dir dir;
	tinydir_open(&dir, folder.c_str());
	while(dir.has_next) {
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if(file.is_dir == false && file.name[0] != '.' && filename_extension_is_image_type(get_extension_from_filename(file.name))) {
			tImgFnames.push_back(file.name);
			tImgPaths.push_back(file.path);
			numFoundImages++;
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	
	if(numFoundImages <= max_number_of_images_to_load) {
		for(int ii=0; ii<numFoundImages; ii++) {
			returned_image_filenames.push_back(tImgFnames[ii]);
			newarr_writehere = new std::vector<unsigned char>();
			cv::imencode(".png", cv::imread(tImgPaths[ii],CV_LOAD_IMAGE_COLOR), *newarr_writehere, pngparam);
			returned_png_images.push_back(newarr_writehere);
			compressedSizeOfAllImages += (newarr_writehere->size());
			cout<<"found "<<returned_png_images.size()<<" images so far, total size: "<<static_cast<int>(((double)compressedSizeOfAllImages)*0.001*0.001)<<" megabytes"<<endl;
		}
	}
	else {
		while(returned_png_images.size() != max_number_of_images_to_load)
		{
			RNG_rand_r rngg;
			int newidx = rngg.rand_int(0, ((int)tImgPaths.size()) - 1);
			
			newarr_writehere = new std::vector<unsigned char>();
			cv::imencode(".png", cv::imread(tImgPaths[newidx],CV_LOAD_IMAGE_COLOR), *newarr_writehere, pngparam);
			compressedSizeOfAllImages += (newarr_writehere->size());
			
			returned_png_images.push_back(newarr_writehere);
			returned_image_filenames.push_back(tImgFnames[newidx]);
			
			tImgPaths.erase( tImgPaths.begin() + newidx);
			tImgFnames.erase(tImgFnames.begin() + newidx);
			
			cout<<"chose image \""<<returned_image_filenames.back()<<"\" for study, total size so far: "<<static_cast<int>(((double)compressedSizeOfAllImages)*0.001*0.001)<<" megabytes"<<endl;
		}
	}
	assert(returned_png_images.size() == returned_image_filenames.size());
}
