#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "line.h"
#include "polygon.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor blue   = TGAColor(0, 0,   255,   255);
const TGAColor green   = TGAColor(0, 255,  0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

int main(int argc, char** argv) {
	TGAImage lineImage(100, 100, TGAImage::RGB);
	TGAImage triangleImage(200, 200, TGAImage::RGB);
	TGAImage barytriangleImage(200, 200, TGAImage::RGB);

	Vec2i t0[3] = {Vec2i(50, 160), Vec2i(10, 70), Vec2i(70, 80)};
	Polygon::triangle(t0, triangleImage, red, false);
	Polygon::barycentricPolygonRenderer(t0, barytriangleImage, red);

	Vec2i t1[3] = {Vec2i(150, 1),  Vec2i(180, 50), Vec2i(70, 180)};
	Polygon::triangle(t1, triangleImage, white, false);
	Polygon::barycentricPolygonRenderer(t1, barytriangleImage, white);

	// sorted by y-coordinate: {(150,1), (180, 50), (70, 180)}

	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
	Polygon::triangle(t2, triangleImage, green, false);
	Polygon::barycentricPolygonRenderer(t2, barytriangleImage, green);

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
	image.write_tga_file("outputWireframe.tga");

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
	image2.write_tga_file("outputWireframeAA.tga");

	TGAImage flatShadingRandomBary(width, height, TGAImage::RGB);
	TGAImage flatShadingRandomScanline(width, height, TGAImage::RGB);

	for (int i=0; i<model->nfaces(); i++) { 
    	std::vector<int> face = model->face(i); 
    	Vec2i screen_coords[3]; 
    	for (int j=0; j<3; j++) { 
        	Vec3f world_coords = model->vert(face[j]); 
        	screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
    	} 
    	Polygon::barycentricPolygonRenderer(screen_coords, flatShadingRandomBary, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
		Polygon::triangle(screen_coords, flatShadingRandomScanline, TGAColor(rand()%255, rand()%255, rand()%255, 255)); 
	}

	flatShadingRandomBary.flip_vertically();
	flatShadingRandomBary.write_tga_file("outputFlatShadingBary.tga");

	flatShadingRandomScanline.flip_vertically();
	flatShadingRandomScanline.write_tga_file("outputFlatShadingScanline.tga");

	TGAImage flatShadingWithLighting(width, height, TGAImage::RGB);
	Vec3f light_dir(0,0,-1); // define light_dir

	for (int i=0; i<model->nfaces(); i++) { 
    	std::vector<int> face = model->face(i); 
    	Vec2i screen_coords[3]; 
    	Vec3f world_coords[3]; 
    	for (int j=0; j<3; j++) { 
        	Vec3f v = model->vert(face[j]); 
        	screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.); 
        	world_coords[j]  = v; 
    	} 
    	Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
    	n.normalize(); 
    	float intensity = n*light_dir; 
    	if (intensity>0) { 
	        Polygon::triangle(screen_coords, flatShadingWithLighting, TGAColor(intensity*255, intensity*255, intensity*255, 255)); 
    	} 
	}

	flatShadingWithLighting.flip_vertically();
	flatShadingWithLighting.write_tga_file("outputFlatShadingLighting.tga");

	delete model;
	return 0;
}
