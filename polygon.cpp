#include <iostream>
#include "polygon.h"
#include "line.h"
#include "color.h"
#include "geometry.h"

Vec3f Polygon::getTriangleNormal(Vec3f vertices[])
{
	return (vertices[1] - vertices[0])^(vertices[2] - vertices[0]);
}

float Polygon::getTriangleArea(Vec3f triangleNormal) 
{
	return triangleNormal.norm() / 2;
}

Vec3f Polygon::getSubtriangleNormal(Vec3f v0, Vec3f v1, Vec3f point) {
	return (v0 - v1)^(point - v1);
}

float Polygon::calculateBaryArea(Vec3f subtriangleNrm) 
{
	return (subtriangleNrm.norm() / 2);
}

// For now: assuming that the vertices array is length 3
bool Polygon::isPointInsideTriangle(Vec2i vertices[], Vec2i point, Vec3f triangleNormal, float triangleArea)
{
	// Now is there a way we can avoid recalculating this over and over?
	// The triangleNormal/area will always stay the same for one set of vertices
	// Vec3f verts3D[] = {Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(vertices[2].x, vertices[2].y, 0)};
	// Vec3f triangleNormal = getTriangleNormal(verts3D);
	// float area = getTriangleArea(triangleNormal);

	// calculating normal vector of subtriangle 1
	Vec3f subtriangleNrm1 = getSubtriangleNormal(Vec3f(vertices[2].x, vertices[2].y, 0), Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(point.x, point.y, 0));
	// calculating normal vector of subtriangle 2
	Vec3f subtriangleNrm2 = getSubtriangleNormal(Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(vertices[2].x, vertices[2].y, 0), Vec3f(point.x, point.y, 0));
	// calculating normal vector of subtriangle 3
	Vec3f subtriangleNrm3 = getSubtriangleNormal(Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(point.x, point.y, 0));
	// if (!(triangleNormal*subtriangleNrm1 < 0 || triangleNormal*subtriangleNrm2 < 0 || triangleNormal*subtriangleNrm3 < 0)) {
	// 	std::cout << vertices[0] << vertices[1] << vertices[2] << point << std::endl;
	// 	std::cout << u << " " << v << " " << w << std::endl;
	// 	std::cout << (float)vertices[0].x*u + (float)vertices[1].x*v + (float)vertices[2].x*w << " " << vertices[0].y*u + vertices[1].y*v + vertices[2].y*w << std::endl;
	// 	std::cout << "----------------------------------------" << std::endl;
	// }

	/* We can imagine u_Nrm, v_Nrm and w_Nrm as the normal vectors of subtriangles of P
	If P exists in the triangle then each subtriangle would have their vectors facing the same direction as the main triangle. 
	Otherwise, one of the subtriangles is outside since it is pointing the opposite direction and therefore it doesn't exist inside. */
	return triangleNormal*subtriangleNrm1 < 0 || triangleNormal*subtriangleNrm2 < 0 || triangleNormal*subtriangleNrm3 < 0 ? false : true;
}

bool Polygon::shouldDrawPixel(Vec3f vertices[], Vec2i point, Vec3f triangleNormal, float triangleArea, int *zbuffer, int idx) {
	Vec3f subtriangleNrm1 = getSubtriangleNormal(Vec3f(vertices[2].x, vertices[2].y, 0), Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(point.x, point.y, 0));
	float u = calculateBaryArea(subtriangleNrm1) / triangleArea;

	// calculating normal vector of subtriangle 2
	Vec3f subtriangleNrm2 = getSubtriangleNormal(Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(vertices[2].x, vertices[2].y, 0), Vec3f(point.x, point.y, 0));
	float v = calculateBaryArea(subtriangleNrm2) / triangleArea;

	// calculating normal vector of subtriangle 3
	Vec3f subtriangleNrm3 = getSubtriangleNormal(Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(point.x, point.y, 0));
	float w = calculateBaryArea(subtriangleNrm3) / triangleArea;

	int zCoord = u*vertices[0].z + v*vertices[1].z + w*vertices[2].z;

	if (triangleNormal*subtriangleNrm1 < 0 || triangleNormal*subtriangleNrm2 < 0 || triangleNormal*subtriangleNrm3 < 0) return false;

	if (zCoord > zbuffer[idx])
	{
		zbuffer[idx] = zCoord;
		return true;
	}

	return false;
}

template <class t> BoundingBox Polygon::findBoundingBox(Vec2<t> vertices[])
{
	int smallestX = std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x));
	int maxWidth = std::max(abs(smallestX - vertices[0].x), std::max(abs(smallestX - vertices[1].x), abs(smallestX - vertices[2].x)));

	int smallestY = std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y));
	int maxHeight = std::max(abs(smallestY - vertices[0].y), std::max(abs(smallestY - vertices[1].y), abs(smallestY - vertices[2].y)));

	return { smallestX, smallestY, maxWidth, maxHeight };
}

void Polygon::barycentricPolygonRenderer(Vec2i vertices[], TGAImage &image, TGAColor color)
{
	BoundingBox bbox = findBoundingBox(vertices);

	Vec3f verts3D[] = {Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(vertices[2].x, vertices[2].y, 0)};
	Vec3f triangleNormal = getTriangleNormal(verts3D);
	float area = getTriangleArea(triangleNormal);

	// maxWidth/maxHeight represent the max distance from the smallestX/smallestY
	// We therefore need to add them together to find the endpoint of the bounding box
	for (int x = bbox.smallestX; x <= (bbox.smallestX + bbox.maxWidth); x++)
	{
		for (int y = bbox.smallestY; y <= (bbox.smallestY + bbox.maxHeight); y++)
		{
			if (isPointInsideTriangle(vertices, Vec2i(x, y), triangleNormal, area) == true)
			{
				image.set(x,y,color);
			}
		}
	}
}


/* Here is the strategy for hidden surface removal
If you use barycentric coordinates, you can multiply the barycentric coordinates by each vertices' x and y coordinate
and sum the resulting x-coordinates and y-coordinates
to find the corresponding point inside the triangle (ie. 0.33,0.33,0.33 means a point at the middle of the triangle).
Therefore, to find the z-coordinate, we apply the exact the same logic, but multiplying the barycentric coordinate to each vertices' z coordinate!
Once we find the z-coordinate, it becomes easy. We just compare with the z-buffer and if the z-coordinate > zbuffer value 
then we replace and set the pixel otherwise ignore.
*/
void Polygon::barycentricPolygonRenderer(Vec3f vertices[], TGAImage &image, TGAColor color, int *zbuffer, int width) {
	Vec2i bboxCoords[] = {Vec2i(vertices[0].x, vertices[0].y), Vec2i(vertices[1].x, vertices[1].y), Vec2i(vertices[2].x, vertices[2].y)};
	BoundingBox bbox = findBoundingBox(bboxCoords);

	Vec3f verts3D[] = {Vec3f(vertices[0].x, vertices[0].y, 0), Vec3f(vertices[1].x, vertices[1].y, 0), Vec3f(vertices[2].x, vertices[2].y, 0)};
	Vec3f triangleNormal = getTriangleNormal(verts3D);
	float area = getTriangleArea(triangleNormal);

	// 800 x 600 = 480,000
	// x=0,y=0 idx = 0
	// x=25,y=25 idx = 25 + 25*800 = 20,025

	// width = # of columns (x)
	// height = # of rows (y)
	// idx = x + y * width
	// x is the starting point in row major
	// y * width means how many rows we need to offset
	
	// int x = idx % width;
	// int y = idx / width;

	// maxWidth/maxHeight represent the max distance from the smallestX/smallestY
	// We therefore need to add them together to find the endpoint of the bounding box
	for (int x = bbox.smallestX; x <= (bbox.smallestX + bbox.maxWidth); x++)
	{
		for (int y = bbox.smallestY; y <= (bbox.smallestY + bbox.maxHeight); y++)
		{
			int idx = x + y*width;
			if (shouldDrawPixel(vertices, Vec2i(x,y), triangleNormal, area, zbuffer, idx)) {
				image.set(x, y, color);
				loggingCalls++;
			}
			// TODO: here is where we compare with zbuffer to determine if we show or don't show the pixel
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
void Polygon::scanline(Vec2i t[], TGAImage &image, TGAColor color, bool useAA) {
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
    sortPolygonByYCoordinates(t);

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

void Polygon::drawTriangle(TriangleArgs args) {
	Polygon triangle(args.width, args.height);

	for (auto v : args.t) {
		if (args.useBary) {
			// https://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array
			barycentricPolygonRenderer(&v.v[0], *triangle.image, v.tColor);	
		} else {
			scanline(&v.v[0], *triangle.image, v.tColor);
		}
	}

	triangle.image->flip_vertically();
	std::string fileName = args.useBary ? "outputBaryTriangle" : "outputTriangle";
	fileName += "_"+std::to_string(args.width)+"_"+std::to_string(args.height);
	fileName += "_"+std::to_string(rand() % 1000);
	fileName += ".tga";
	triangle.image->write_tga_file(fileName.c_str());

	delete triangle.image;
}	

// Uncomment out the scanline code if needed
// Will use barycentric for the time being
void Polygon::drawFlatShadingRandom(Model *model, FlatShadingArgs args) {
	Polygon flatShadingRandomBary;
	// Polygon flatShadingRandomScanline;

	for (int i=0; i<model->nfaces(); i++) {
    	std::vector<int> face = model->face(i);
    	Vec2i screen_coords[3];
    	for (int j=0; j<3; j++) {
        	Vec3f world_coords = model->vert(face[j]); 
        	screen_coords[j] = Vec2i((world_coords.x+1.)*args.width/2., (world_coords.y+1.)*args.height/2.); 
    	} 
    	barycentricPolygonRenderer(screen_coords, *flatShadingRandomBary.image, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
		// scanline(screen_coords, *flatShadingRandomScanline.image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
	}

	flatShadingRandomBary.image->flip_vertically();
	flatShadingRandomBary.image->write_tga_file("outputFlatShadingBary.tga");

	// flatShadingRandomScanline.image->flip_vertically();
	// flatShadingRandomScanline.image->write_tga_file("outputFlatShadingScanline.tga");

	delete flatShadingRandomBary.image;
	// delete flatShadingRandomScanline.image;
}

// Uncomment out the scanline code if needed
// Will use barycentric for the time being
void Polygon::drawFlatShadingWithLighting(Model *model, FlatLightingArgs args) {
	// Polygon flatShadingWithLighting;
	Polygon flatBaryShadingWithLighting;

	int width = flatBaryShadingWithLighting.width;
	int height = flatBaryShadingWithLighting.height;

	int *zbuffer = new int[width*height];
	for (int i=0; i<width*height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

	for (int i=0; i<model->nfaces(); i++) {
    	std::vector<int> face = model->face(i); 
    	Vec2i screen_coords[3]; 
		Vec3f screen_coords_z[3];
    	Vec3f world_coords[3]; 
    	for (int j=0; j<3; j++) { 
        	Vec3f v = model->vert(face[j]); 
        	screen_coords[j] = Vec2i((v.x+1.)*args.width/2., (v.y+1.)*args.height/2.);
			screen_coords_z[j] = Vec3f((v.x+1.)*args.width/2., (v.y+1.)*args.height/2., v.z);
        	world_coords[j]  = v; 
    	} 
    	Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
    	n.normalize(); 
    	float intensity = n*args.lightDir;
    	if (intensity>0) { 
			// std::cout << screen_coords_z[0] << screen_coords_z[1] << screen_coords_z[2] << std::endl;
	        // scanline(screen_coords, *flatShadingWithLighting.image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
			// barycentricPolygonRenderer(screen_coords, *flatBaryShadingWithLighting.image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
			barycentricPolygonRenderer(screen_coords_z, *flatBaryShadingWithLighting.image, TGAColor(intensity*255, intensity*255, intensity*255, 255), zbuffer, width);
    	}
	}

	std::cout << loggingCalls << std::endl;

	// flatShadingWithLighting.image->flip_vertically();
	// flatShadingWithLighting.image->write_tga_file("outputFlatShadingLighting.tga");

	flatBaryShadingWithLighting.image->flip_vertically();
	flatBaryShadingWithLighting.image->write_tga_file("outputFlatBaryShadingLighting.tga");

	// delete flatShadingWithLighting.image;
	delete flatBaryShadingWithLighting.image;
}