#ifndef __LINE_H__
#define __LINE_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

namespace Line {
    void addPtsToVector(std::vector<Vec2i> &pts, double newX, double newY);
    void DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, std::vector<Vec2i> &linePts);
    void bresenham(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void xiaolinAntiAliasing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
};

#endif