#include <iostream>
#include "line.h"

void Line::addPtsToVector(std::vector<Vec2i> &pts, double newX, double newY)
{
	int ptsSize = pts.size();
	if (ptsSize == 0) pts.push_back(Vec2i(newX, newY));

	// We're checking if [ptsSize-1].y != newY because we only want unique Y-values for scanline rendering
	else if (ptsSize > 0 && pts[ptsSize-1].y != newY) {
		pts.push_back(Vec2i(newX, newY));
	}
}

void Line::DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	double steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	double x_inc = dx / steps;
	double y_inc = dy / steps;

	double x = x0;
	double y = y0;

	for (int i = 0; i <= steps; i++)
	{
		image.set(round(x), round(y), color);
		x += x_inc;
		y += y_inc;
	}
}

void Line::DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, std::vector<Vec2i> &linePts)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	double steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	double x_inc = dx / steps;
	double y_inc = dy / steps;

	double x = x0;
	double y = y0;

	for (int i = 0; i <= steps; i++)
	{
		addPtsToVector(linePts, round(x), round(y));
		x += x_inc;
		y += y_inc;
	}
}

void Line::bresenham(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	// this algorithm avoids floating point multiplication to make it super fast
	// only uses addition/substraction which is much faster than multiplication (I did not know this)

	// imagine we're doing 0,0 to 50,50

	int dx = abs(x1 - x0); // 50
	int sx = 1;
	int sy = 1;
	if (x0 < x1)
	{
		sx = 1; // sx = 1
	} else {
		sx = -1;
	}

	int dy = -abs(y1 - y0); // -50
	if (y0 < y1) 
	{
		sy = 1; // sy = 1
	} else {
		sy = -1;
	}

	int e = dx + dy; // 0

	// image.set(0,0)
	// image.set(1,1)
	// image.set(2,2)
	// ...

	while (1) {
		image.set(x0, y0, color); 	
		if (x0 == x1 && y0 == y1){
			break;
		}

		int e2 = 2*e; // 0
		if (e2 >= dy) {
			if (x0 == x1) break;

			e += dy;  // -50
			x0 += sx; // 1
		}

		if (e2 <= dx) {
			if (y0 == y1) break;

			e += dx; // 0
			y0 += sy; // 1
		}
	}
}

void Line::xiaolinAntiAliasing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;

	double steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	double x_inc = dx / steps;
	double y_inc = dy / steps;

	double x = x0;
	double y = y0;

	for (int i = 0; i <= steps; i++)
	{
		// so here, we need to check the distance between the adjacent pixels
		// we're essentially doing linear interpolation

		// we don't need both of these, we need to check which increment is not 1
		// because of the "steps" logic above, one of x_inc or y_inc will be 1
		float wholeY = floor(y);
		float wholeX = floor(x);
		
		// this means we have a decimal part and verifying the y-axis
		if (x_inc == 1 && y - wholeY != wholeY) {
			// 1 - (y - wholeY) gives us the distance between the upper pixel and our point
			// (y - wholeY) gives us the distance between the bottom pixel and our point

			// however the color alpha would be the inverse of the distance
			// ie. if the distance from the top is 30% then that pixel should have an intensity of 70%

			// TODO: extract this?
			float topIntensity = y - wholeY;
			float bottomIntensity = 1 - (y - wholeY);

			// TODO: this can be extracted out into a new function
			TGAColor topColor = TGAColor(color.r* topIntensity, color.g* topIntensity, color.b* topIntensity, color.a);
			TGAColor bottomColor = TGAColor(color.r* bottomIntensity, color.g* bottomIntensity, color.b* bottomIntensity, color.a);


			// TODO: we should be able to extract this out as well
			if (y - wholeY >= 0.5) { // means rounding gives us the top pixel
				image.set(x, round(y), topColor);
				image.set(x, round(y) - 1, bottomColor);
			} else { // rounding gives us the bottom pixel
				image.set(x, round(y) + 1, topColor);
				image.set(x, round(y), bottomColor);
			}
		} else if (y_inc == 1 && x - wholeX != wholeX) {
			float rightIntensity = x - wholeX;
			float leftIntensity = 1 - (x - wholeX);

			TGAColor rightColor = TGAColor(color.r * rightIntensity, color.g * rightIntensity, color.b * rightIntensity, color.a);
			TGAColor leftColor = TGAColor(color.r * leftIntensity, color.g * leftIntensity, color.b * leftIntensity, color.a);

			if (x - wholeX >= 0.5) {
				image.set(round(x), y, rightColor);
				image.set(round(x) - 1, y, leftColor);
			} else {
				image.set(round(x) + 1, y, rightColor);
				image.set(round(x), y, leftColor);
			}
		} else {
			image.set(x, y, color);
		}

		x += x_inc;
		y += y_inc;
	}
}

void Line::drawRandomLines(int w, int h, int lineCount) {
	LineImage line(w, h);

	for (int i = 0; i < lineCount; i++)
	{
		xiaolinAntiAliasing(rand() % line.width, rand() % line.height, rand() % line.width, rand() % line.height, *line.lineImage, line.color.red);
		DDA(rand() % line.width, rand() % line.height, rand() % line.width, rand() % line.height, *line.lineImage, line.color.green);
		bresenham(rand() % line.width, rand() % line.height, rand() % line.width, rand() % line.height, *line.lineImage, line.color.blue);
	}

	line.lineImage->flip_vertically(); // i want to have the origin at the left bottom corner of the image
	std::string fileName = "outputRandomLines";
	fileName += "_"+std::to_string(w);
	fileName += "_"+std::to_string(h);
	fileName += "_"+std::to_string(lineCount);
	fileName += ".tga";
	line.lineImage->write_tga_file(fileName.c_str());
}

void Line::drawStarburst() {
	// Coming Soon
}