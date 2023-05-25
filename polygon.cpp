#include <iostream>
#include "polygon.h"
#include "line.h"

// For now: assuming that the vertices array is length 3
bool Polygon::isPointInsideTriangle(Vec2i vertices[], Vec2i point)
{
	double baryCenter1Numerator = ((vertices[1].y - vertices[2].y) * (point.x - vertices[2].x)) + ((vertices[2].x - vertices[1].x)*(point.y - vertices[2].y));
	double denominator = ((vertices[1].y - vertices[2].y) * (vertices[0].x - vertices[2].x)) + ((vertices[2].x - vertices[1].x)*(vertices[0].y - vertices[2].y));
	
	float baryCenter1 = baryCenter1Numerator / denominator;
	bool bary1InsideTriangle = baryCenter1 <= 1 && baryCenter1 >= 0;

	double baryCenter2Numerator = ((vertices[2].y - vertices[0].y)*(point.x - vertices[2].x)) + ((vertices[0].x - vertices[2].x)*(point.y - vertices[2].y));
	float baryCenter2 = baryCenter2Numerator / denominator;
	bool bary2InsideTriangle = baryCenter2 <= 1 && baryCenter2 >= 0;

	float baryCenter3 = 1 - baryCenter1 - baryCenter2;
	bool bary3InsideTriangle = baryCenter3 <= 1 && baryCenter3 >= 0;

	return bary1InsideTriangle && bary2InsideTriangle && bary3InsideTriangle ? true : false;
}

void Polygon::barycentricPolygonRenderer(Vec2i vertices[], TGAImage &image, TGAColor color)
{
	int smallestX = std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x));
	int maxWidth = std::max(abs(smallestX - vertices[0].x), std::max(abs(smallestX - vertices[1].x), abs(smallestX - vertices[2].x)));

	int smallestY = std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y));
	int maxHeight = std::max(abs(smallestY - vertices[0].y), std::max(abs(smallestY - vertices[1].y), abs(smallestY - vertices[2].y)));

	// maxWidth/maxHeight represent the max distance from the smallestX/smallestY
	// We therefore need to add them together to find the endpoint of the bounding box
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

void Polygon::sortPolygonByYCoordinates(Vec2i vertices[])
{
    // Most efficient sorting algorithm would be merge sort, but let's just do a simple one for now
    // Insertion Sort
    // Assuming polygon has maximum 3 vertices
    int i = 1;
    while (i < 3)
    {
        int indexToDecrement = i;
        while (indexToDecrement > 0) {
            if (vertices[indexToDecrement].y < vertices[indexToDecrement-1].y)
            {
                //swap
                Vec2i tmp = vertices[indexToDecrement];
                vertices[indexToDecrement] = vertices[indexToDecrement-1];
                vertices[indexToDecrement-1] = tmp;
            }
            indexToDecrement--;
        }

        i++;
    }
}

// TODO: add some comments for your thought process then add it to the README
void Polygon::triangle(Vec2i t[], TGAImage &image, TGAColor color, bool useAA) {
	/* The idea to use vectors is to keep track of all points
	to be drawn on the lines so that we'd be able to draw a horizontal line between the points */
	
	/* The lesson learned here was that slopes of each line are not guaranteed to be the same
	therefore we need to ensure the horizontal lines being drawn have matching y-coordinates.
	If you try to naïvely add every single point to the vector and then draw horizontal lines
	you'll notice that quite a lot of gaps appear simply because the y-coordinates aren't matching */
	std::vector<Vec2i> line1Pts;
	std::vector<Vec2i> line2Pts;
	std::vector<Vec2i> line3Pts;
	
	/* THIS IS VERY IMPORTANT so that we can maintain consistency for rendering the triangle in halves */
    Polygon::sortPolygonByYCoordinates(t);

	// Applying AA to the lines (WIP)
	if (useAA) {
		Line::xiaolinAntiAliasing(t[0].x, t[0].y, t[2].x, t[2].y, image, color);
		Line::xiaolinAntiAliasing(t[0].x, t[0].y, t[1].x, t[1].y, image, color);
		Line::xiaolinAntiAliasing(t[1].x, t[1].y, t[2].x, t[2].y, image, color);
	}

	/* 
	With the vertices ordered by y-coordinates, we can control how the scanline rendering works
	
	Basically what's going on here is:
	line1Pts are the points going from the bottom-most vertex to the top-most one
	line2Pts are the points going from the bottom-most vertex to the middle one
	line3Pts are the points going from the middle vertex to the top-most one

	With this information, we can render the bottom half of the triangle with horizontal lines
	from line1Pts to line2Pts stopping once we reach the last yCoordinate of line2Pts.
	
	From this point, we continue rendering the top half of the triangle by 
	using line3Pts and the continuation of line1Pts.
	*/
	Line::DDA(t[0].x, t[0].y, t[2].x, t[2].y, image, color, line1Pts);
	Line::DDA(t[0].x, t[0].y, t[1].x, t[1].y, image, color, line2Pts);
	Line::DDA(t[1].x, t[1].y, t[2].x, t[2].y, image, color, line3Pts);

	int line1PtsSize = line1Pts.size();
	int line2PtsSize = line2Pts.size();
	int yCoordinateLimit = line2Pts[line2PtsSize - 1].y; // Because of the sorting, line2Pts is ALWAYS going to the middle which is the endpoint of the first halve
	int inc = 0;

	// Rendering the first halve of the triangle
	while (line2Pts[inc].y < yCoordinateLimit)
	{
		Line::DDA(line1Pts[inc].x, line1Pts[inc].y, line2Pts[inc].x, line2Pts[inc].y, image, color);
		inc++;
	}

	// Rendering the second halve of the triangle
	int incLine3 = 0;
	while (inc < line1PtsSize && line1Pts[inc].y <= t[2].y) // t2.y is the top point of the triangle so we can just use that
	{
		Line::DDA(line1Pts[inc].x, line1Pts[inc].y, line3Pts[incLine3].x, line3Pts[incLine3].y, image, color);
		inc++;
		incLine3++;
	}

	line1Pts.clear();
	line2Pts.clear();
	line3Pts.clear();
}



