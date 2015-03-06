#ifndef MESSAGE_HANDLING_H_
#define MESSAGE_HANDLING_H_

#include <zmq.hpp>

#include "Backbone/IMGData.hpp"
#include "Backbone/Backbone.hpp"

int messageSizeNeeded(imgdata_t *data);
unsigned char *linearizeData(imgdata_t *data, int *retlen);
void expandData(imgdata_t *data, unsigned char *arr);
void packMessageData(zmq::message_t *msg, imgdata_t *data);
void unpackMessageData(imgdata_t *data, zmq::message_t *msg);

#endif
