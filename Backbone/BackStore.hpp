#ifndef BACKSTORE_H
#define BACKSTORE_H

#include "Backbone/IMGData.hpp"

bool img_update(imgdata_t& data);
bool img_delete(imgdata_t data);
void img_print(imgdata_t data);

#endif
