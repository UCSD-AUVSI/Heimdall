#ifndef ___GEOREFERENCE_HPP______
#define ___GEOREFERENCE_HPP______

#include <vector>
#include <math.h>
#include "SharedUtils/SharedUtils.hpp"

std::pair<double, double> georeference_target_in_image(int targetrow,
							int targetcol,
							int imrows,
							int imcols,
							double planelat,
							double planelongt,
							double planeheading,
							double planealtitude);

#endif
