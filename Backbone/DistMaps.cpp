#include <string>
#include <map>

#include "Backbone/DistMaps.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

//
// GENERAL MAPS
//

//Mapping between classes of algorithms and actual implementations
const std::map<std::string, std::vector<std::string>> selectMap =
{
	{"orgr", 		{"STUB_ORGR"}},
	{"saliency", 	{"STUB_SALIENCY", "GUISAL"}},
	{"seg",			{"BLOB", "EROSION", "GUIREC"}},
	{"srec", 		{"TEMPLATE"}},
	{"ocr", 		{"TESSERACT"}},
	{"verif", 		{"STUB_VERIF"}}
};

//Mapping between strings referring to algorithms
//and in code references to algorithms
const std::map<std::string, alg_t> algStrMap = 
{
	{"STUB_ORGR", 		STUB_ORGR},
	{"STUB_SALIENCY", 	STUB_SALIENCY},
	{"GUISAL",			GUISAL},
	{"GUIREC",			GUIREC},
	{"BLOB",			BLOB_SEG},
	{"EROSION",			EROSION_SEG},
	{"TEMPLATE",		TEMPLATE_SREC},
	{"TESSERACT", 		TESS_OCR},
	{"STUB_VERIF", 		STUB_VERIF},
	{"NONE",			NONE}
};

//Mapping between strings and algorithm classes
const std::map<std::string, algclass_t> algClassStrMap =
{
	{"orgr", 		ORGR},
	{"saliency", 	SALIENCY},
	{"seg", 		SEG},
	{"srec",		SREC},
	{"ocr", 		OCR},
	{"verif", 		VERIF}
};

//
// DISTRIBUTED CLIENT SIDE MAPS
//

//Mapping between algorithms, and what ports they use from client-side
//First port in list is what it pulls from
//Rest are what they push to
const std::map<alg_t, std::vector<zmqport_t>> distPortMap =
{
	{STUB_ORGR,		{IMAGES_PUSH, ORGR_PULL}},
	{STUB_SALIENCY,	{ORGR_PUSH, SALIENCY_PULL}},
	{GUISAL,		{ORGR_PUSH, SALIENCY_PULL}},
	{GUIREC,		{SALIENCY_PUSH, TARGET_PULL}},
	{BLOB_SEG,		{SALIENCY_PUSH, SEG_PULL}},
	{EROSION_SEG,	{SALIENCY_PUSH, SEG_PULL}},
	{TEMPLATE_SREC,	{SSEG_PUSH, TARGET_PULL}},
	{TESS_OCR,		{CSEG_PUSH, TARGET_PULL}},
	{STUB_VERIF,	{TARGET_PUSH, VERIFIED_PULL}},
	{NONE,			{}}
};

//Mapping between algorithms, and the actual classes that implement them
const std::map<alg_t, void (*)(imgdata_t *)> algFuncMap =
{
	{STUB_ORGR,		Stub_ORGRF :: execute},
	{STUB_SALIENCY,	Stub_Saliency :: execute},
	{GUISAL,		GUISaliency :: execute},
	{GUIREC,		GUIRec :: execute},	
	{BLOB_SEG,  	BlobSeg :: execute},
	{EROSION_SEG,	ErosionSeg :: execute},
	{TEMPLATE_SREC, TemplateSRec :: execute},
	{TESS_OCR, 		TessOCR :: execute},
	{STUB_VERIF,	Stub_Verify :: execute},
	{NONE,			0}
};
