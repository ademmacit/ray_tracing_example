#ifndef COLOR_H
#define COLOR_H

#include "rtweekend.h"

#include "vec3.h"

#include <iostream>

#include "stb_image.h"
#include "stb_image_write.h"

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	//divide the color by the number of samples AND gamma=correct for gamma = 2
	// - we can use “gamma 2” which means raising the color to the power 1/gamma, or in our simple case ½, which is just square-root-
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	//write the translated [0,255] value for each color component
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << "\n";

}

//for stb_image library use
void write_color_stb(int &index,uint8_t* pixels,color pixel_color,int samples_per_pixel) {

	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	//divide the color by the number of samples AND gamma=correct for gamma = 2
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	pixels[index++] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
	pixels[index++] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
	pixels[index++] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
}

#endif // !COLOR_H
