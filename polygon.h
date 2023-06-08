#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "image.h"
#include "model.h"

struct TProperties {
    std::vector<Vec2i> v;
    TGAColor tColor;
};

struct BaseArgs {
    int width = BaseImage::width;
    int height = BaseImage::height;
    int bpp = BaseImage::bpp;
};

struct TriangleArgs : BaseArgs {
    std::vector<TProperties> t;
    bool useBary = false;
    bool useAA = false;
};

struct FlatShadingArgs : BaseArgs {};

struct FlatLightingArgs : BaseArgs {
    Vec3f lightDir = Vec3f(0,0,-1);
};

namespace Polygon {
    class Polygon : public BaseImage::Image {
        using Image::Image;
    };

    int findYCoordinateLimit(std::vector<Vec2i> &pts1, std::vector<Vec2i> &pts2, int pts1Size, int pts2Size);
    bool isPointInsideTriangle(Vec2i vertices[], Vec2i point);
    void barycentricPolygonRenderer(Vec2i vertices[], TGAImage &image, TGAColor color);
    void barycentricPolygonRenderer(Vec3f vertices[], TGAImage &image, TGAColor color);
    void scanline(Vec2i t[], TGAImage &image, TGAColor color, bool useAA = false);
    void sortPolygonByYCoordinates(Vec2i vertices[]);

    void drawTriangle(TriangleArgs args);
    void drawFlatShadingRandom(Model *model, FlatShadingArgs args = {});
    void drawFlatShadingWithLighting(Model *model, FlatLightingArgs args);
}

#endif