#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const point& origin, const vec3& direction)
		: orig(origin), dir(direction) {}
	
	point origin() const { return orig; }
	point direction() const { return dir; }

	point at(double t) const {
		return orig + t * dir;
	}
public:
	point orig;
	vec3 dir;
};

#endif // !RAY_H
#pragma once
