#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"
#include "hittable.h"

class sphere : public hittable
{
public:
	sphere() {}
	sphere(point cen, double r ,shared_ptr<material> m) :
		center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(
		const ray& r, double tmin, double tmax, hit_record& rec) const override;

public:
	point center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center;
	//ORIGIN -vector dotted with itself is equal to the squared length of that vector-
	auto a = r.direction().length_squared();
	//RAY DIRECTION -half_b * 2 = ray direction -
	auto half_b = dot(r.direction(), oc);
	
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;


	if (discriminant > 0) {
		auto root = sqrt(discriminant);

		auto temp = (-half_b - root) / a;
		if (temp <t_max && temp> t_min)
		{
			rec.t = temp;
			rec.p = r.at(rec.t);

			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;

			return true;
		}

		temp = (-half_b + root) / a;
		if (temp <t_max && temp> t_min)
		{
			rec.t = temp;
			rec.p = r.at(rec.t);

			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;

			return true;
		}
	}
	return false;
}


#endif