#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "line.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor blue   = TGAColor(0, 0,   255,   255);
const TGAColor green   = TGAColor(0, 255,  0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

int findYCoordinateLimit(std::vector<Vec2i> &pts1, std::vector<Vec2i> &pts2, int pts1Size, int pts2Size)
{
	if (std::min(pts1Size, pts2Size) == pts2Size) {
		return pts2[pts2Size - 1].y; 
	}
	return pts1[pts1Size - 1].y;
}

bool isPointInsideTriangle(Vec2i vertices[], Vec2i point)
{
	// Need error handling to ensure we have at most 3 vertices
	// if (vertices != 3) return false;

	double baryCenter1Numerator = ((vertices[1].y - vertices[2].y) * (point.x - vertices[2].x)) + ((vertices[2].x - vertices[1].x)*(point.y - vertices[2].y));
	double denominator = ((vertices[1].y - vertices[2].y) * (vertices[0].x - vertices[2].x)) + ((vertices[2].x - vertices[1].x)*(vertices[0].y - vertices[2].y));
	
	float baryCenter1 = baryCenter1Numerator / denominator;
	bool bary1InsideTriangle = baryCenter1 <= 1 && baryCenter1 >= 0;

	double baryCenter2Numerator = ((vertices[2].y - vertices[0].y)*(point.x - vertices[2].x)) + ((vertices[0].x - vertices[2].x)*(point.y - vertices[2].y));
	float baryCenter2 = baryCenter2Numerator / denominator;
	bool bary2InsideTriangle = baryCenter2 <= 1 && baryCenter2 >= 0;

	// std::cout << baryCenter1 << " " << baryCenter2 << " " << 1 - baryCenter1 - baryCenter2 << std::endl;
	float baryCenter3 = 1 - baryCenter1 - baryCenter2;
	bool bary3InsideTriangle = baryCenter3 <= 1 && baryCenter3 >= 0;

	// std::cout << point << 1 - baryCenter1 - baryCenter2 << std::endl;
	return bary1InsideTriangle && bary2InsideTriangle && bary3InsideTriangle ? true : false;
}

void barycentricPolygonRenderer(Vec2i vertices[], TGAImage &image, TGAColor color)
{
	int smallestX = std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x));
	int maxWidth = std::max(abs(smallestX - vertices[0].x), std::max(abs(smallestX - vertices[1].x), abs(smallestX - vertices[2].x)));

	int smallestY = std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y));
	int maxHeight = std::max(abs(smallestY - vertices[0].y), std::max(abs(smallestY - vertices[1].y), abs(smallestY - vertices[2].y)));

	for (int x = smallestX; x <= (smallestX + maxWidth); x++)
	{
		for (int y = smallestY; y <= (smallestY + maxHeight); y++)
		{
			if (isPointInsideTriangle(vertices, Vec2i(x, y)) == true)
			{
				image.set(x,y,color);
			}
		}
	}
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color, bool useAA = false) { 
	std::vector<Vec2i> line1Pts;
	std::vector<Vec2i> line2Pts;
	std::vector<Vec2i> line3Pts;
	
	// sorted by y-coordinate: {(10,70), (70, 80), (50, 160)}
	// TODO: we need to handle the direction (CW or CCW) and be able to render it the same
	// But first let's handle this case and then do the other case after

	// This will be very inefficient, but so be it
	if (useAA) {
		Line::xiaolinAntiAliasing(t0.x, t0.y, t2.x, t2.y, image, color);
		Line::xiaolinAntiAliasing(t0.x, t0.y, t1.x, t1.y, image, color);
		Line::xiaolinAntiAliasing(t1.x, t1.y, t2.x, t2.y, image, color);
	}

	Line::DDA(t0.x, t0.y, t2.x, t2.y, image, color, line1Pts);
	Line::DDA(t0.x, t0.y, t1.x, t1.y, image, color, line2Pts);
	Line::DDA(t1.x, t1.y, t2.x, t2.y, image, color, line3Pts);

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
			// The reason we see no AA is because scanline rendering just draws horizontal lines
			// You need to apply xiaolin AA on the outlines FIRST
			Line::DDA(line1Pts[inc].x, line1Pts[inc].y, line2Pts[inc].x, line2Pts[inc].y, image, color);
			inc++;
		}
	} else {
		while (line2Pts[inc].y < yCoordinateLimit)
		{
			Line::DDA(line1Pts[inc].x, line1Pts[inc].y, line2Pts[inc].x, line2Pts[inc].y, image, color);
			inc++;
		}
	}

	// we now need to draw horizontal lines between lines1Pts and lines3Pts

	int incLine3 = 0;
	while (inc < line1PtsSize && line1Pts[inc].y <= t2.y) // t2.y is the top point of the triangle so we can just use that
	{
		Line::DDA(line1Pts[inc].x, line1Pts[inc].y, line3Pts[incLine3].x, line3Pts[incLine3].y, image, color);
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
	TGAImage barytriangleImage(200, 200, TGAImage::RGB);
	// image.set(75, 75, white);
	// image.set(25, 75, blue);
	// line(100,0,50,50,image,red);
	// line(0,0,50,50,image, blue);
	// line(0,100,20,50,image, white);
	// bresenham(70,50,100,100,image, blue);
	// bresenham(100,100,50,50,image, red);

	// line(10,70,50,160,triangleImage, red);
	// line(70,80,50,160,triangleImage, red);

	Vec2i t0[3] = {Vec2i(10, 70), Vec2i(70, 80), Vec2i(50, 160)};
	triangle(t0[0], t0[1], t0[2], triangleImage, red, false);
	barycentricPolygonRenderer(t0, barytriangleImage, red);

	Vec2i t1[3] = {Vec2i(150, 1),  Vec2i(180, 50), Vec2i(70, 180)};
	triangle(t1[0], t1[1], t1[2], triangleImage, white, false);
	barycentricPolygonRenderer(t1, barytriangleImage, white);

	// sorted by y-coordinate: {(150,1), (180, 50), (70, 180)}

	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
	triangle(t2[0], t2[1], t2[2], triangleImage, green, false);
	barycentricPolygonRenderer(t2, barytriangleImage, green);

	// already sorted, this might not always be the case however

	barytriangleImage.flip_vertically();
	barytriangleImage.write_tga_file("outputBaryTriangle.tga");

	triangleImage.flip_vertically();
	triangleImage.write_tga_file("outputTriangle.tga");

	Line::xiaolinAntiAliasing(50,0,100,90,lineImage, white);
	Line::DDA(0,20,100,70,lineImage, white);
	Line::bresenham(0,0,90,50,lineImage, white);

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
            Line::bresenham(x0, y0, x1, y1, image, white);
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
			Line::xiaolinAntiAliasing(x0, y0, x1, y1, image2, white);
        }
    }

	image2.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image2.write_tga_file("output2.tga");

	// TODO:
	// 1. Bresenham is stuck in an infinite loop?
	// 2. It's finally time to start sorting all your vertices by y-coordinates
	TGAImage flatShadingRandom(width, height, TGAImage::RGB);
	for (int i=0; i<model->nfaces(); i++) { 
    	std::vector<int> face = model->face(i); 
    	Vec2i screen_coords[3]; 
    	for (int j=0; j<3; j++) { 
        	Vec3f world_coords = model->vert(face[j]); 
        	screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
    	} 
    	barycentricPolygonRenderer(screen_coords, flatShadingRandom, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
	}

	flatShadingRandom.flip_vertically();
	flatShadingRandom.write_tga_file("flatShading.tga");

	delete model;
	return 0;
}
