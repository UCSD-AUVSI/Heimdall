#pragma once

#include <vector>
#include <string>

extern const std::vector</*const*/ std::string> truth_file_target_entry_keywords;

class TruthFile_TargetInImage
{
public:
	//there are a number of values equal to the number of elements in "truth_file_target_entry_keywords"
	std::vector<std::string> entry_values;
	
	TruthFile_TargetInImage();
};

class TruthFile_ImageEntry
{
public:
	std::string image_file;
	
	std::vector<TruthFile_TargetInImage> targets_in_image;
};

class TruthFile
{
public:
	std::vector<TruthFile_ImageEntry> images;
};


bool LoadTruthFile(std::string truthFilename, TruthFile & returnedTruth);
