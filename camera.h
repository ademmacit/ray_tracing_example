#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera
{
public:
	camera(
		point lookfrom,
		point lookat,
		vec3 vup,
		double vfov, // vertical field of view in degrees
		double aspect_ratio,
		double aperture, // virtual lens size for defocus blur
		double focus_dist // distance to focus plane for defocus blur
	) {
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		// vup - world up
		w = unit_vector(lookfrom - lookat); // -camera to back
		u = unit_vector(cross(vup, w));	 // -camera to right
		v = cross(w, u);					 // -camera to up

		origin = lookfrom;
		horizontal = focus_dist* viewport_width * u;
		vertical = focus_dist* viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist*w;

		lens_radius = aperture / 2;

		};

	ray get_ray(double s,double t) const {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();

		return ray(origin+offset,
			lower_left_corner + s * horizontal + t * vertical - origin - offset
		);
	}
private:
	point origin;
	point lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	double lens_radius;
	
};


#endif // !CAMERA_H
