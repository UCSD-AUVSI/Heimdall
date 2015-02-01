#pragma once

#include <vector>
#include <string>

extern const std::vector<std::string> truthfile_target_entrykeywords;
extern const std::vector<std::string> truthfile_falsepos_entrykeywords;


class TruthFile_TargetInImage {
public:
	//there are a number of values equal to the number of elements in "truthfile_target_entrykeywords"
	std::vector<std::string> entry_values;
	TruthFile_TargetInImage();
};

class TruthFile_FalsePositiveInImage {
public:
	//there are a number of values equal to the number of elements in "truthfile_falsepos_entrykeywords"
	std::vector<std::string> entry_values;
	TruthFile_FalsePositiveInImage();
};

class TruthFile_ImageEntry {
public:
	std::string image_file;
	std::vector<TruthFile_TargetInImage> targets_in_image;
	std::vector<TruthFile_FalsePositiveInImage> falsepositives_in_image;
};

class TruthFile {
public:
	std::vector<TruthFile_ImageEntry> images;
};


bool LoadTruthFile(std::string truthFilename, TruthFile & returnedTruth);

std::string GetTruthEntryValue(std::string entryName, TruthFile_TargetInImage target);
std::string GetTruthEntryValue(std::string entryName, TruthFile_FalsePositiveInImage obj);
