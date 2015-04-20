# Heimdall

Repository for UCSD AUVSI computer vision software suite

## Notes

#### EXIF data required for georeferencing by the Saliency (Salient Object Detection) module:

* `GPSLatitude`
* `GPSLongitude`
* `GPSAltitude`             (relative to Mean Sea Level MSL)
* `GPSSpeed`                hack: should contain mean ground level altitude (assuming a flat field)
* `GPSImgDirection`         compass direction in degrees where 0 == North, 90 == East, 180 == South, 270 == West

#### Format of messages passed along the vision processing flowchart (from algorithm to algorithm):

see `Backbone/IMGData.hpp`

the actual images are encoded to vectors of unsigned chars (i.e. uint8_t) so need to be decoded back to cv::Mat if you want to process them

#### Algorithms, Modules, and the vision processing flowchart:

see `Backbone/DistMaps.cpp`

note that you will not have to modify this unless you wrote C++ code or want to reorganize the flowchart

Heimdall Vision Flowchart 2014:

![alt text](https://github.com/UCSD-AUVSI/Heimdall/blob/master/heimdall_vision_flowchart_2014.png "Heimdall Vision Flowchart 2014")

