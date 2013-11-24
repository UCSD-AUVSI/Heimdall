#include <string>
#include <map>

#include "Backbone/Maps.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

//Mapping between classes of algorithms and actual implementations
const std::map<std::string, std::vector<std::string>> selectMap =
{
	{"orgr", 		{"ORGR"}},
	{"saliency", 	{"SALIENCY", "GUISAL"}},
	{"seg",			{"BLOB", "EROSION", "GUIREC"}},
	{"srec", 		{"TEMPLATE"}},
	{"ocr", 		{"TESSERACT"}},
	{"verif", 		{"VERIF"}}
};

//Mapping between strings referring to algorithms
//and in code references to algorithms
const std::map<std::string, alg_t> strMap = 
{
	{"ORGR", 		ORGR},
	{"SALIENCY", 	SALIENCY},
	{"GUISAL",		GUISAL},
	{"GUIREC",		GUIREC},
	{"BLOB",		BLOB_SEG},
	{"EROSION",		EROSION_SEG},
	{"TEMPLATE",	TEMPLATE_SREC},
	{"TESSERACT", 	TESS_OCR},
	{"VERIF", 		VERIF},
	{"NONE",		NONE}
};

//Mapping between algorithms, and what ports they use
//First port in list is what it pulls from
//Rest are what they push to
const std::map<alg_t, std::vector<zmqport_t>> portMap =
{
	{ORGR,			{IMAGES_PUSH, ORGR_PULL}},
	{SALIENCY,		{ORGR_PUSH, SALIENCY_PULL}},
	{GUISAL,		{ORGR_PUSH, SALIENCY_PULL}},
	{GUIREC,		{SALIENCY_PUSH, TARGET_PULL}},
	{BLOB_SEG,		{SALIENCY_PUSH, SEG_PULL}},
	{EROSION_SEG,	{SALIENCY_PUSH, SEG_PULL}},
	{TEMPLATE_SREC,	{SSEG_PUSH, TARGET_PULL}},
	{TESS_OCR,		{CSEG_PUSH, TARGET_PULL}},
	{VERIF,			{TARGET_PUSH, VERIFIED_PULL}},
	{NONE,			{}}
};

const std::map<alg_t, void (*)(imgdata_t *)> algMap =
{
	{ORGR,			ORGRF :: execute},
	{SALIENCY,		Saliency :: execute},
	{GUISAL,		GUISaliency :: execute},
	{GUIREC,		GUIRec :: execute},	
	{BLOB_SEG,  	BlobSeg :: execute},
	{EROSION_SEG,	ErosionSeg :: execute},
	{TEMPLATE_SREC, TemplateSRec :: execute},
	{TESS_OCR, 		TessOCR :: execute},
	{VERIF,			Verify :: execute},
	{NONE,			0}
};
