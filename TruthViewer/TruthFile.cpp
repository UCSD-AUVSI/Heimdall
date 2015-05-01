#include "TruthFile.hpp"
#include <iostream>
#include <fstream>
#include "SharedUtils/SharedUtils.hpp"
using std::cout; using std::endl;



/*extern*/ const std::vector<std::string> truthfile_falsepos_entrykeywords = {
	"\tpos_x: ",
	"\tpos_y: ",
	"\tbox_min_width: ",
	"\tbox_min_height: "
};
/*extern*/ const std::vector<std::string> truthfile_target_entrykeywords = {
	"\tshape: ",
	"\tshape_color: ",
	"\tcharacter: ",
	"\tcharacter_color: ",
	"\tpos_x: ",
	"\tpos_y: ",
	"\tbox_min_width: ",
	"\tbox_min_height: "
};
static const std::string image_entry_keyword	= "Image: ";
static const std::string target_entry_keyword	= "  Target:";
static const std::string falsepos_entry_keyword	= "  FalsePositive:";
static const std::string qrcode_entry_keyword	= "  QRCode:";


std::string GetTruthEntryValue(std::string entryName, TruthFile_TargetInImage target)
{
	std::string firstChars = entryName.substr(entryName.size()-1);
	if(firstChars != std::string(":")) {
		firstChars = entryName.substr(entryName.size()-2);
		if(firstChars != std::string(": ")) {
			entryName = (entryName + ":");
		}
	}
	for(int i=0; i<truthfile_target_entrykeywords.size(); i++) {
		if(contains_substr_i(truthfile_target_entrykeywords[i], entryName)) {
			return target.entry_values[i];
		}
	}
	cout << "ERROR in TruthFile.cpp, GetTruthEntryValue(): could not find truth entry \"" << entryName << "\" for target" << endl;
	return std::string();
}
std::string GetTruthEntryValue(std::string entryName, TruthFile_FalsePositiveInImage fp_obj)
{
	std::string firstChars = entryName.substr(entryName.size()-1);
	if(firstChars != std::string(":")) {
		firstChars = entryName.substr(entryName.size()-2);
		if(firstChars != std::string(": ")) {
			entryName = (entryName + ":");
		}
	}
	for(int i=0; i<truthfile_falsepos_entrykeywords.size(); i++) {
		if(contains_substr_i(truthfile_falsepos_entrykeywords[i], entryName)) {
			return fp_obj.entry_values[i];
		}
	}
	cout << "ERROR in TruthFile.cpp, GetTruthEntryValue(): could not find truth entry \"" << entryName << "\" for false positive" << endl;
	return std::string();
}


TruthFile_TargetInImage::TruthFile_TargetInImage() {
	entry_values.resize(truthfile_target_entrykeywords.size());
}
TruthFile_FalsePositiveInImage::TruthFile_FalsePositiveInImage() {
	entry_values.resize(truthfile_falsepos_entrykeywords.size());
}


static std::string FixWindowsEOLInString(std::string linestr) {
	if(linestr[linestr.size()-1] == '\r') {
		return linestr.substr(0,linestr.size()-1);
	}
	return linestr;
}


bool LoadTruthFile(std::string truthFilename, TruthFile & returnedTruth)
{
	const int CONSOLE_VERBOSITY = 4;
	bool foundFile;
	std::string line;
	int lengthOfKeyword;
	int numKeywordsFoundValidForTarget;
	bool oneOfTheKeywordsMatched;
	std::ifstream tfile(truthFilename);
	if(tfile.is_open()) {
		foundFile = true;
		
		//start loop to look for all images
		while(line.empty()==false || std::getline(tfile,line)) {
			if(  line.size() > image_entry_keyword.size()
			  && line.substr(0,image_entry_keyword.size()) == image_entry_keyword)
			{
				//we have an image
				returnedTruth.images.push_back(TruthFile_ImageEntry());
				returnedTruth.images.back().image_file = FixWindowsEOLInString(line.substr(image_entry_keyword.size()));
				consoleOutput.Level(CONSOLE_VERBOSITY)<<"FOUND IMAGE: \""<<returnedTruth.images.back().image_file<<"\""<<endl;
				line.clear();
				
				//start loop to look for all targets and false positives in this image
				while(line.empty()==false || std::getline(tfile,line)) {
					line = FixWindowsEOLInString(line);
					if(line == target_entry_keyword) {
						consoleOutput.Level(CONSOLE_VERBOSITY)<<"FOUND TARGET IN IMAGE \""<<returnedTruth.images.back().image_file<<"\""<<endl;
						//we have a target; read its properties
						returnedTruth.images.back().targets_in_image.push_back(TruthFile_TargetInImage());
						numKeywordsFoundValidForTarget = 0;
						
						//start loop to look for descriptive entries for object
						while(std::getline(tfile,line)) {
							oneOfTheKeywordsMatched = false;
							if(numKeywordsFoundValidForTarget < truthfile_target_entrykeywords.size()) {
								for(int i=0; i<truthfile_target_entrykeywords.size(); i++) {
									lengthOfKeyword = truthfile_target_entrykeywords[i].size();
									if(line.size() > lengthOfKeyword) {
										if(line.substr(0,lengthOfKeyword) == truthfile_target_entrykeywords[i]) {
											oneOfTheKeywordsMatched = true;
											numKeywordsFoundValidForTarget++;
											returnedTruth.images.back().targets_in_image.back().entry_values[i] = FixWindowsEOLInString(line.substr(lengthOfKeyword));
											continue;
										}
									}
								}
							}
							if(oneOfTheKeywordsMatched == false) {
								if(numKeywordsFoundValidForTarget < truthfile_target_entrykeywords.size()) {
									std::cout << "error in reading truth file \"" << truthFilename << "\": error reading target in \""
											  << returnedTruth.images.back().image_file << "\"" << std::endl;
								}
								break;
							}
						}
					} else if(line == falsepos_entry_keyword || line == qrcode_entry_keyword) {
						if(line == falsepos_entry_keyword) {
							consoleOutput.Level(CONSOLE_VERBOSITY)<<"FOUND FALSE POSITIVE IN IMAGE \""<<returnedTruth.images.back().image_file<<"\""<<endl;
						} else {
							consoleOutput.Level(CONSOLE_VERBOSITY)<<"FOUND QR CODE IN IMAGE \""<<returnedTruth.images.back().image_file<<"\"... treating as false positive since ShapeRec, OCR, etc. will fail"<<endl;
						}
						
						//we have a false positive; read its properties
						returnedTruth.images.back().falsepositives_in_image.push_back(TruthFile_FalsePositiveInImage());
						numKeywordsFoundValidForTarget = 0;
						
						//start loop to look for descriptive entries for object
						while(std::getline(tfile,line)) {
							oneOfTheKeywordsMatched = false;
							if(numKeywordsFoundValidForTarget < truthfile_falsepos_entrykeywords.size()) {
								for(int i=0; i<truthfile_falsepos_entrykeywords.size(); i++) {
									lengthOfKeyword = truthfile_falsepos_entrykeywords[i].size();
									if(line.size() > lengthOfKeyword) {
										if(line.substr(0,lengthOfKeyword) == truthfile_falsepos_entrykeywords[i]) {
											oneOfTheKeywordsMatched = true;
											numKeywordsFoundValidForTarget++;
											returnedTruth.images.back().falsepositives_in_image.back().entry_values[i] = FixWindowsEOLInString(line.substr(lengthOfKeyword));
											continue;
										}
									}
								}
							}
							if(oneOfTheKeywordsMatched == false) {
								if(numKeywordsFoundValidForTarget < truthfile_falsepos_entrykeywords.size()) {
									std::cout << "error in reading truth file \"" << truthFilename << "\": error reading false positive in \""
											  << returnedTruth.images.back().image_file << "\"" << std::endl;
								}
								break;
							}
						}
					} else {
						//usually this contains the next "Image:" line
						break;
					}
				}
			} else {
				line.clear(); //nothing interesting in this line; get another line to read
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
