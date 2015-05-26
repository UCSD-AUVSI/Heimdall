# Heimdall

Repository for UCSD AUVSI computer vision software suite

## Notes

#### (Partial) List of Libraries Needed To Build

cmake
libopencv-dev
libzmq-dev
python-dev
libboost-python-dev
libtesseract-dev
libleptonica-dev

Optional: QT widgets 5

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

Heimdall Vision Flowchart 2015:

![alt text](https://github.com/UCSD-AUVSI/Heimdall/blob/master/2015heimdall_wbg_qrcode.png "Heimdall Vision Flowchart 2015")

#### Algorithms that can be written in Python:

* Salient Object Detection
* QR Code Read/Check
* Segmentation
* OCR
* Results Aggregation

Not supported: Python-Shape-Recognition (though the current C++ implementation is good)

