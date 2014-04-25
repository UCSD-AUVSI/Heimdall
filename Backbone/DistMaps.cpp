#include <string>
#include <map>
#include <vector>

#include "Backbone/DistMaps.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

//
// MODULE CREATION
//

//Mapping between classes of algorithms and actual implementations
const std::map<std::string, std::vector<std::string>> alg_choice_map =
{
    {"images",      {"SINGLE_FILE_PUSH", "NONE"}},
    {"orthorect",   {"STUB_ORTHORECT", "NONE"}},
    {"saliency",    {"SSALIENCY", "STUB_SALIENCY", "GUISAL", "NONE"}},
    {"seg",         {"BLOB_SEG", "GUIREC", "NONE"}},
    {"srec",        {"TEMPLATE_SREC", "NONE"}},
    {"ocr",         {"TESS_OCR", "NONE"}},
    {"verif",       {"STUB_VERIF", "NONE"}}
};

//Mapping between algorithms, and the actual classes that implement them
const std::map<std::string, void (*)(imgdata_t *, std::string)> alg_func_map =
{
    {"SINGLE_FILE_PUSH",    FilePush :: execute},
    {"STUB_ORTHORECT",      Stub_Orthorect :: execute},
    {"STUB_SALIENCY",       Stub_Saliency :: execute},
    {"SSALIENCY",           SSaliency :: execute},
    {"GUISAL",              GUISaliency :: execute},
    {"GUIREC",              GUIRec :: execute}, 
    {"BLOB_SEG",            BlobSeg :: execute},
    {"TEMPLATE_SREC",       TemplateSRec :: execute},
    {"TESS_OCR",            TessOCR :: execute},
    {"STUB_VERIF",          Stub_Verify :: execute},
    {"NONE",                0}
};
;
