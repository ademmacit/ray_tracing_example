#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>

//USINGS

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//CONSTANTS

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//UTILITY FUNCTIONS

inline double degrees_to_radians(double degree) {

	return degree * pi / 180.0;
}

inline double random_double(){
	// returns a random real [0,1)
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	// returns a random real [min,max)
	return min + (max-min)*random_double();
}

inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;

}

//COMMON HEADERS

#include "ray.h"
#include "vec3.h"


#endif // !RTWEEKEND_H
