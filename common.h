#ifndef COMMON
#define COMMON

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <math.h>
#include <string>
#include <cstring>

#define XRES 2048
#define YRES 1536
#define FOV 50

struct color
{
	int r, g, b;
};

color operator+(color a, color b);
color operator*(color a, double b);
color operator*(color a, color b);

color average_color(color*, int);
color clamp_color(color);

#endif
