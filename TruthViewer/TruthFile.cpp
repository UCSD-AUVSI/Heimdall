#include "TruthFile.hpp"
#include <iostream>
#include <fstream>


/*extern*/ const std::vector</*const*/ std::string> truth_file_target_entry_keywords = {
	"\tshape: ",
	"\tshape_color: ",
	"\tcharacter: ",
	"\tcharacter_color: ",
	"\tpos_x: ",
	"\tpos_y: ",
	"\tbox_min_width: ",
	"\tbox_min_height: "
};
static const std::string image_entry_keyword = "Image: ";
static const std::string target_entry_keyword = "  Target:";


TruthFile_TargetInImage::TruthFile_TargetInImage() {
	entry_values.resize(truth_file_target_entry_keywords.size());
}


static std::string FixWindowsEOLInString(std::string linestr) {
	if(linestr[linestr.size()-1] == '\r') {
		return linestr.substr(0,linestr.size()-1);
	}
	return linestr;
}


bool LoadTruthFile(std::string truthFilename, TruthFile & returnedTruth)
{
	bool foundFile;
	std::string line;
	int lengthOfKeyword;
	int numKeywordsFoundValidForTarget;
	bool alreadyHaveNextLine = false;
	bool oneOfTheKeywordsMatched;
	std::ifstream tfile(truthFilename);
	if(tfile.is_open()) {
		foundFile = true;
		while(alreadyHaveNextLine || std::getline(tfile,line)) {
			alreadyHaveNextLine = false;
			if(line.size() > image_entry_keyword.size()) {
				if(line.substr(0,image_entry_keyword.size()) == image_entry_keyword) {
					//we have an image
					returnedTruth.images.push_back(TruthFile_ImageEntry());
					returnedTruth.images.back().image_file = FixWindowsEOLInString(line.substr(image_entry_keyword.size()));
					if(std::getline(tfile,line)) {
						alreadyHaveNextLine = true;
						if(FixWindowsEOLInString(line) == target_entry_keyword) {
							//we have a target; read its properties
							returnedTruth.images.back().targets_in_image.push_back(TruthFile_TargetInImage());
							numKeywordsFoundValidForTarget = 0;
							while(std::getline(tfile,line)) {
								oneOfTheKeywordsMatched = false;
								if(numKeywordsFoundValidForTarget < truth_file_target_entry_keywords.size()) {
									for(int i=0; i<truth_file_target_entry_keywords.size(); i++) {
										lengthOfKeyword = truth_file_target_entry_keywords[i].size();
										if(line.size() > lengthOfKeyword) {
											if(line.substr(0,lengthOfKeyword) == truth_file_target_entry_keywords[i]) {
												oneOfTheKeywordsMatched = true;
												numKeywordsFoundValidForTarget++;
												returnedTruth.images.back().targets_in_image.back().entry_values[i] = FixWindowsEOLInString(line.substr(lengthOfKeyword));
												break;
											}
										}
									}
								}
								if(oneOfTheKeywordsMatched == false) {
									if(numKeywordsFoundValidForTarget < truth_file_target_entry_keywords.size()) {
										std::cout << "error in reading truth file \"" << truthFilename << "\": error reading target in \""
												  << returnedTruth.images.back().image_file << "\"" << std::endl;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		foundFile = false;
		std::cout << "WARNING: Could not open truth file \"" << truthFilename << "\" !" << std::endl;
	}
	
	tfile.close();
	return foundFile;
}

