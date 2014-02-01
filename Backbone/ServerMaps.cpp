#include <string>
#include <map>

#include "Backbone/ServerMaps.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

//
// SERVER SIDE MAPS
// MODIFICATIONS UNNECESSARY FOR MODULE ADDITION
//

//Only one pull port per thread allowed
const std::map<int, std::vector<zmqport_t>> serverPullPortMap =
{
	{0, {IMAGES_PULL}}, //Image Thread
	{1, {ORGR_PULL}}, //ORGR Thread
	{2, {SALIENCY_PULL}}, //Saliency Thread
	{3, {SEG_PULL}}, //Segmentation Thread
	{4, {TARGET_PULL}}, //Target Thread
	{5,	{VERIFIED_PULL}}  //Verification Thread
};

const std::map<int, std::vector<zmqport_t>> serverPushPortMap =
{
	{0, {IMAGES_PUSH}}, //Image Thread
	{1, {ORGR_PUSH}}, //ORGR Thread
	{2, {SALIENCY_PUSH}}, //Saliency Thread
	{3, {CSEG_PUSH, SSEG_PUSH}}, //Segmentation Thread
	{4, {TARGET_PUSH}}, //Target Thread
	{5,	{NO_PORT}}  //Verification Thread
};

// Only one publish port per thread allowed
const std::map<int, std::vector<zmqport_t>> serverPubPortMap =
{
	{0, {IMAGES_PUB}}, //Image Thread
	{1, {NO_PORT}}, //ORGR Thread
	{2, {SALIENCY_PUB}}, //Saliency Thread
	{3, {NO_PORT}}, //Segmentation Thread
	{4, {TARGET_PUB}}, //Target Thread
	{5,	{VERIFIED_PUB}}  //Verification Thread
};
