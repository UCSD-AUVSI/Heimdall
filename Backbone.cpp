#include <string>
#include <map>

#include "Maps.hpp"
#include "Backbone.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

//Mapping between strings referring to algorithms
//and in code references to algorithms
const std::map<std::string, alg_t> strMap = 
{
	{"Orthorect", 	ORTHORECT},
	{"GeoRef", 		GEOREF},
	{"Saliency", 	SALIENCY},
	{"Seg",			SEG},
	{"SSeg", 		SSEG},
	{"CSeg", 		CSEG},
	{"SRec", 		SREC},
	{"OCR", 		OCR},
	{"Verif", 		VERIF}
};

//Mapping between algorithms, and what ports they use
//First port in list is what it pulls from
//Rest are what they push to
const std::map<alg_t, std::vector<zmqport_t>> portMap =
{
	{ORTHORECT,	{IMAGES_PUSH, ORTHORECT_PULL}},
	{GEOREF,	{ORTHORECT_PUSH, GEOREF_PULL}},
	{SALIENCY,	{GEOREF_PUSH, SALIENCY_PULL}},
	{SEG,		{SALIENCY_PUSH, S_SEG_PULL, C_SEG_PULL}},
	{SSEG,		{SALIENCY_PUSH, S_SEG_PULL}},
	{CSEG, 		{SALIENCY_PUSH, C_SEG_PULL}},
	{SREC,		{S_SEG_PUSH, TARGET_PULL}},
	{OCR,		{C_SEG_PUSH, TARGET_PULL}},
	{VERIF,		{TARGET_PUSH, VERIFIED_PULL}}
};

const std::map<alg_t, void (*)(imgdata_t &)> algMap =
{
	{ORTHORECT, Orthorect :: execute},
	{GEOREF,	GeoRef :: execute},
	{SALIENCY,	Saliency :: execute},
	{SEG,		Segmentation :: execute},
	{SSEG,		ShapeSegmentation :: execute},
	{CSEG, 		CharacterSegmentation :: execute},
	{SREC,		ShapeRecognition :: execute},
	{OCR,		CharacterRecognition :: execute},
	{VERIF,		Verify :: execute}
};
