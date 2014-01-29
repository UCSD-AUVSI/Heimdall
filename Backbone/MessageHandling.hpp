#include <zmq.hpp>

#include "Backbone/IMGData.hpp"
#include "Backbone/Backbone.hpp"

int messageSizeNeeded(imgdata_t *data);
unsigned char *linearizeData(imgdata_t *data, int *retlen);
void packMessageData(zmq::message_t *msg, imgdata_t *data);
void expandData(imgdata_t *data, unsigned char *arr);
void unpackMessageData(imgdata_t *data, zmq::message_t *msg);
