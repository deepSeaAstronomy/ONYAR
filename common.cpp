#include <common.h>

color operator+(color a, color b)
{
	color retCol;

	retCol.r = a.r + b.r;
	retCol.g = a.g + b.g;
	retCol.b = a.b + b.b;

	return retCol;
}

color operator*(color a, double b)
{
	color retCol;

	retCol.r = a.r * b;
	retCol.g = a.g * b;
	retCol.b = a.b * b;

	return retCol;
}

color operator*(color a, color b)
{
	color retCol;

	retCol.r = a.r * (double(b.r) / 255);
	retCol.g = a.g * (double(b.g) / 255);
	retCol.b = a.b * (double(b.b) / 255);

	return retCol;
}
color clamp_color(color retCol)
{
	if (retCol.r > 255)
		retCol.r = 255;

	if (retCol.g > 255)
		retCol.g = 255;

	if (retCol.b > 255)
		retCol.b = 255;

	if (retCol.r < 0)
		retCol.r = 0;

	if (retCol.g < 0)
		retCol.g = 0;

	if (retCol.b < 0)
		retCol.b = 0;

	return retCol;
}

color average_color(color* colors, int numColors)
{
	color retColor; retColor.r = 0; retColor.g = 0; retColor.b = 0;

	for (int i = 0; i < numColors; i++)
		retColor = retColor + colors[i];

	return retColor * (1.0f / numColors);
}
