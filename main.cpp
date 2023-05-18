#include <iostream>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor blue   = TGAColor(0, 0,   255,   255);
const TGAColor green   = TGAColor(0, 255,  0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

// TODO: don't forget to clear these vectors when you're done
std::vector<Vec2i> line1Pts;
std::vector<Vec2i> line2Pts;
std::vector<Vec2i> line3Pts;

void addPtsToVector(std::vector<Vec2i> &pts, double newX, double newY)
{
	int ptsSize = pts.size();
	if (ptsSize == 0) pts.push_back(Vec2i(newX, newY));
	else if (ptsSize > 0 && pts[ptsSize-1].y != newY) {
		pts.push_back(Vec2i(newX, newY));
	}
}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, int lineToAdd = -1)
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
		if (lineToAdd == -1) image.set(round(x), round(y), color);
		else {
			if (lineToAdd == 1) {
				// TODO: extract this out, it is getting too crazy
				addPtsToVector(line1Pts, round(x), round(y));
			} else if (lineToAdd == 2) {
				addPtsToVector(line2Pts, round(x), round(y));
			} 
			else {
				addPtsToVector(line3Pts, round(x), round(y));
			}
		}

		x += x_inc;
		y += y_inc;
	}
}

void bresenham(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
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

void xiaolinAntiAliasing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
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

int findYCoordinateLimit(std::vector<Vec2i> &pts1, std::vector<Vec2i> &pts2, int pts1Size, int pts2Size)
{
	if (std::min(pts1Size, pts2Size) == pts2Size) {
		return pts2[pts2Size - 1].y; 
	}
	return pts1[pts1Size - 1].y;
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) { 
	// sorted by y-coordinate: {(10,70), (70, 80), (50, 160)}
	// TODO: we need to handle the direction (CW or CCW) and be able to render it the same
	// But first let's handle this case and then do the other case after

	line(t0.x, t0.y, t2.x, t2.y, image, color, 1);
	line(t0.x, t0.y, t1.x, t1.y, image, color, 2);
	line(t1.x, t1.y, t2.x, t2.y, image, color, 3);

	int yCoordinateLimit;
	int line1PtsSize = line1Pts.size();
	int line2PtsSize = line2Pts.size();

	yCoordinateLimit = findYCoordinateLimit(line1Pts, line2Pts, line1PtsSize, line2PtsSize);
	int inc = 0;

	// Can we have two separate loops storing the unique points and then use the other loop to find the y coordinates that match
	// In order to do this we need to figure out which line has the steeper slope
	// The line with the steeper slope will obviously hit the limit faster

	// we don't need to find the steeper slope, in the line function above, I simply take all Vec2i where the y values are unique

	// TODO: this could be extracted
	if (line1Pts[line1PtsSize - 1].y == yCoordinateLimit)
	{
		while (line1Pts[inc].y < yCoordinateLimit) 
		{
			line(line1Pts[inc].x, line1Pts[inc].y, line2Pts[inc].x, line2Pts[inc].y, image, color);
			inc++;
		}
	} else {
		while (line2Pts[inc].y < yCoordinateLimit)
		{
			line(line1Pts[inc].x, line1Pts[inc].y, line2Pts[inc].x, line2Pts[inc].y, image, color);
			inc++;
		}
	}

	// we now need to draw horizontal lines between lines1Pts and lines3Pts

	int incLine3 = 0;
	while (line1Pts[inc].y < t2.y) // t2.y is the top point of the triangle so we can just use that
	{
		line(line1Pts[inc].x, line1Pts[inc].y, line3Pts[incLine3].x, line3Pts[incLine3].y, image, color);
		inc++;
		incLine3++;
	}

	line1Pts.clear();
	line2Pts.clear();
	line3Pts.clear();

	// Great job this works! However this only handles scenarios where the lowest y coordinate is on the left
	// We need to handle the scenario where it is on the right

	// Also you must sort your vertices too, you are assuming that the pts are coming in sorted already
}

int main(int argc, char** argv) {
	TGAImage lineImage(100, 100, TGAImage::RGB);
	TGAImage triangleImage(200, 200, TGAImage::RGB);
	// image.set(75, 75, white);
	// image.set(25, 75, blue);
	// line(100,0,50,50,image,red);
	// line(0,0,50,50,image, blue);
	// line(0,100,20,50,image, white);
	// bresenham(70,50,100,100,image, blue);
	// bresenham(100,100,50,50,image, red);

	// line(10,70,50,160,triangleImage, red);
	// line(70,80,50,160,triangleImage, red);

	Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(70, 80), Vec2i(50, 160)}; 
	triangle(t0[0], t0[1], t0[2], triangleImage, red);

	Vec2i t1[3] = {Vec2i(150, 1),  Vec2i(180, 50), Vec2i(70, 180)};
	triangle(t1[0], t1[1], t1[2], triangleImage, white);
	// sorted by y-coordinate: {(150,1), (180, 50), (70, 180)}

	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
	triangle(t2[0], t2[1], t2[2], triangleImage, green);
	// already sorted
	
	
	// triangle(t1[0], t1[1], t1[2], triangleImage, white); 
	// triangle(t2[0], t2[1], t2[2], triangleImage, green);

	triangleImage.flip_vertically();
	triangleImage.write_tga_file("outputTriangle.tga");

	xiaolinAntiAliasing(50,0,100,90,lineImage, white);
	line(0,20,100,70,lineImage, white);
	bresenham(0,0,90,50,lineImage, white);

	lineImage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	lineImage.write_tga_file("outputLine.tga");

	// argc is the argument count
	// argv is the argument vector
	// so what this is saying is we add a file as an argument
	// parse that file with the Model
	// otherwise by default, parse the african_head
	if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

	// TODO: extract this
    TGAImage image(width, height, TGAImage::RGB);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            bresenham(x0, y0, x1, y1, image, white);
        }
    }

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");

	TGAImage image2(width, height, TGAImage::RGB);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
			xiaolinAntiAliasing(x0, y0, x1, y1, image2, white);
        }
    }

	image2.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image2.write_tga_file("output2.tga");

	delete model;
	return 0;
}
