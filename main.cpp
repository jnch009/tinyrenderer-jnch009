#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "line.h"
#include "polygon.h"
#include "color.h"

Model *model = NULL;
Color color;

int main(int argc, char** argv) {
	Line::drawRandomLines(500,500, 15);
	Line::drawRandomLines(2000,1275, 200);
	Line::drawRandomLines(800,600, 50);
	Line::drawRandomLines(1024,768, 75);
	Line::drawRandomLines(2000,2000, 125);

	std::vector<TProperties> trianglesToRender = {
		{{Vec2i(50, 160), Vec2i(10, 70), Vec2i(70, 80)}, color.red},
		{{Vec2i(150, 1),  Vec2i(180, 50), Vec2i(70, 180)}, color.white},
		{{Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 200)}, color.blue},
		{{Vec2i(20, 150), Vec2i(120, 125), Vec2i(200, 200)}, color.green}
	};

	Line::drawStarburst();
	// Designated Initializers (C++20)
	// Polygon::drawTriangle({BaseArgs { .width = 200, .height = 200 }, .t = trianglesToRender});
	// Polygon::drawTriangle({BaseArgs { .width = 200, .height = 200,}, .t = trianglesToRender, .useBary = true});

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
	// Polygon::drawFlatShadingRandom(model);
	Polygon::drawFlatShadingWithLighting(model, {.lightDir=Vec3f(0,0,-1)});

	delete model;

	// Line::draw2DScene();
	// Line::draw1DScene(200);
	
	return 0;
}
