#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "image.h"

namespace Polygon {
    class Polygon : public BaseImage::Image {
        using Image::Image;
    };

    int findYCoordinateLimit(std::vector<Vec2i> &pts1, std::vector<Vec2i> &pts2, int pts1Size, int pts2Size);
    bool isPointInsideTriangle(Vec2i vertices[], Vec2i point);
    void barycentricPolygonRenderer(Vec2i vertices[], TGAImage &image, TGAColor color);
    void triangle(Vec2i t[], TGAImage &image, TGAColor color, bool useAA = false);
    void sortPolygonByYCoordinates(Vec2i vertices[]);

    void drawTriangle();
}

#endif