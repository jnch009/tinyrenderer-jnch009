#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "line.h"
#include "polygon.h"
#include "color.h"

Model *model = NULL;
Color color;
int width  = 800;
int height = 800;

int main(int argc, char** argv) {
	Line::drawRandomLines();
	Line::drawRandomLines(2000,1275, 200);
	Line::drawRandomLines(800,600, 50);
	Line::drawRandomLines(1024,768, 75);
	Line::drawRandomLines(2000,2000, 125);

	TGAImage triangleImage(200, 200, TGAImage::RGB);
	TGAImage barytriangleImage(200, 200, TGAImage::RGB);

	Vec2i t0[3] = {Vec2i(50, 160), Vec2i(10, 70), Vec2i(70, 80)};
	Polygon::triangle(t0, triangleImage, color.red, false);
	Polygon::barycentricPolygonRenderer(t0, barytriangleImage, color.red);

	Vec2i t1[3] = {Vec2i(150, 1),  Vec2i(180, 50), Vec2i(70, 180)};
	Polygon::triangle(t1, triangleImage, color.white, false);
	Polygon::barycentricPolygonRenderer(t1, barytriangleImage, color.white);

	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
	Polygon::triangle(t2, triangleImage, color.green, false);
	Polygon::barycentricPolygonRenderer(t2, barytriangleImage, color.green);

	barytriangleImage.flip_vertically();
	barytriangleImage.write_tga_file("outputBaryTriangle.tga");

	triangleImage.flip_vertically();
	triangleImage.write_tga_file("outputTriangle.tga");

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

	Line::drawWireframe(model, "outputWireframe");
	Line::drawWireframe(model, "outputWireframeAA", "aa");

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
	TGAImage flatBaryShadingWithLighting(width, height, TGAImage::RGB);
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
			Polygon::barycentricPolygonRenderer(screen_coords, flatBaryShadingWithLighting, TGAColor(intensity*255, intensity*255, intensity*255, 255));
    	}
	}

	flatShadingWithLighting.flip_vertically();
	flatShadingWithLighting.write_tga_file("outputFlatShadingLighting.tga");

	flatBaryShadingWithLighting.flip_vertically();
	flatBaryShadingWithLighting.write_tga_file("outputFlatBaryShadingLighting.tga");

	delete model;
	return 0;
}
