#include <string>
#include <map>
#include <vector>

#include "Backbone/Backbone.hpp"
#include "Backbone/Maps.hpp"

//
// PORT MAPPING
// 

//Mapping between strings and algorithm classes
const std::map<std::string, AlgClass> alg_class_str_map =
{
    {"images",      IMAGES},
    {"orthorect",   ORTHORECT},
    {"saliency",    SALIENCY},
    {"seg",         SEG},
    {"qrcode",      QRCODE},
    {"srec",        SREC},
    {"ocr",         OCR},
    {"color",       COLORCLASS},
    {"verif",       VERIF}
};

// Dependencies between algorithm classes. Used to decide port routing
// Order is LTR ({} -> {})
const std::map<std::vector<AlgClass>, std::vector<AlgClass>> alg_class_dependency_map =
{
    {{IMAGES},                  {ORTHORECT}},
    {{ORTHORECT},               {SALIENCY}},
    {{SALIENCY},                {SEG}},
    {{SEG},                     {QRCODE}},
    {{QRCODE},                  {SREC, OCR, COLORCLASS}},
    {{SREC, OCR, COLORCLASS},   {VERIF}},
    {{VERIF},                   {}}
};
