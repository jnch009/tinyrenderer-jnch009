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

struct BoundingBox {
    int smallestX;
    int smallestY;
    int maxWidth;
    int maxHeight;
};

namespace Polygon {
    class Polygon : public BaseImage::Image {
        using Image::Image;
    };

    int findYCoordinateLimit(std::vector<Vec2i> &pts1, std::vector<Vec2i> &pts2, int pts1Size, int pts2Size);
    Vec3f getTriangleNormal(Vec3f vertices[]);
    float getTriangleArea(Vec3f triangleNormal);
    Vec3f getSubtriangleNormal(Vec3f v0, Vec3f v1, Vec3f point);
    float calculateBaryArea(Vec3f subtriangleNrm);
    
    bool isPointInsideTriangle(Vec2i vertices[], Vec2i point, Vec3f triangleNormal, float triangleArea);
    bool shouldDrawPixel(Vec3f vertices[], Vec2i point, Vec3f triangleNormal, float triangleArea, int *zbuffer, int idx);
    void barycentricPolygonRenderer(Vec2i vertices[], TGAImage &image, TGAColor color);
    void barycentricPolygonRenderer(Vec3f vertices[], TGAImage &image, TGAColor color, int *zbuffer, int width);
    void scanline(Vec2i t[], TGAImage &image, TGAColor color, bool useAA = false);
    void sortPolygonByYCoordinates(Vec2i vertices[]);

    template <class t> BoundingBox findBoundingBox(Vec2<t> vertices[]);

    void drawTriangle(TriangleArgs args);
    void drawFlatShadingRandom(Model *model, FlatShadingArgs args = {});
    void drawFlatShadingWithLighting(Model *model, FlatLightingArgs args);
}

#endif