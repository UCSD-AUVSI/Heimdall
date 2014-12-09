#include <string>
#include <map>
#include <vector>

#include "Backbone/DistMaps.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/DistAlgs.hpp"

//
// MODULE CREATION
//

//Mapping between classes of algorithms and actual implementations
const std::map<std::string, std::vector<std::string>> alg_choice_map =
{
    {"images",      {"FOLDER_WATCH", "SINGLE_FILE_PUSH", "FOLDER_PUSH", "STUB_PUSH", "NONE"}},
    {"orthorect",   {"STUB_ORTHORECT", "NONE"}},
    {"saliency",    {"BLOB_SALIENCY", "SSALIENCY", "STUB_SALIENCY", "PYTHON_SALIENCY", "GUISAL", "NONE"}},
    {"seg",         {"SKYNET_SEG", "CLUSTER_SEG", "STUB_SEG", "GUIREC", "NONE"}},
    {"srec",        {"POLYGON_SREC", "STUB_SREC", "NONE"}},
    {"ocr",         {"GOCR_OCR", "TESS_OCR", "STUB_OCR", "NONE"}},
    {"color",       {"COLOR2014", "STUB_COLOR", "NONE"}},
    {"verif",       {"DISPLAY_VERIF", "PLANE_VERIF", "STUB_VERIF", "NONE"}}
};

//Mapping between algorithms, and the actual classes that implement them
const std::map<std::string, void (*)(imgdata_t *, std::string)> alg_func_map =
{
    {"STUB_PUSH",           StubPush :: execute},
    {"SINGLE_FILE_PUSH",    FilePush :: execute},
    {"FOLDER_PUSH",         FolderPush :: execute},
    {"FOLDER_WATCH",        FolderWatch :: execute},

    {"STUB_ORTHORECT",      StubOrthorect :: execute},

    {"STUB_SALIENCY",       StubSaliency :: execute},
    {"SSALIENCY",           SSaliency :: execute},
    {"BLOB_SALIENCY",       BlobSaliency :: execute},
    {"PYTHON_SALIENCY",		PythonSaliency :: execute},
    {"GUISAL",              GUISaliency :: execute},

    {"STUB_SEG",            StubSeg ::execute},
    {"SKYNET_SEG",  	    SkynetSeg :: execute},
    {"CLUSTER_SEG",  	    ClusterSeg :: execute},
    {"GUIREC",              GUIRec :: execute}, 

    {"STUB_SREC",           StubSRec :: execute},
	{"POLYGON_SREC",	    PolygonShapeRec :: execute},

	{"STUB_OCR", 		    StubOCR :: execute},
	{"TESS_OCR", 		    TessOCR :: execute},
	{"GOCR_OCR", 		    GOCRBackboneInterface :: execute},

	{"COLOR2014", 		    ColorClassifier2014 :: execute},
	{"STUB_COLOR", 		    StubColorClassifier :: execute},
	
    {"STUB_VERIF",          StubVerify :: execute},
    {"DISPLAY_VERIF",       DisplayVerify :: execute},
    {"PLANE_VERIF",         PlaneVerify :: execute},

    {"NONE",                0}
};
