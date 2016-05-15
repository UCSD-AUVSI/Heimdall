#include "PHImage.h"

// The rest of the PHImage class is inlined.
// Inlining the destructor makes the compiler unhappy, so it goes here instead

PHImage::~PHImage() {    
    if (!refCount) {
        return; // the image was a dummy
    }

    refCount[0]--;
    if (*refCount <= 0) {
        delete refCount;
        delete[] data;
    }
}
    
