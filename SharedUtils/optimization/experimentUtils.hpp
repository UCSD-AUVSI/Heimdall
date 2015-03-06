#ifndef __EXPERIMENT_UTILS_HPP___
#define __EXPERIMENT_UTILS_HPP___

#include <vector>
#include <string>

void loadImagesIntoCompressedMemory(std::string folder, int max_number_of_images_to_load,
								std::vector<std::vector<unsigned char>*> & returned_png_images,
								std::vector<std::string> & returned_image_filenames);

#endif
