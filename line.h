#ifndef __LINE_H__
#define __LINE_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "color.h"
#include "model.h"
#include "image.h"

namespace Line {
    inline int randomLines = 25;
    inline Color color;

    class LineImage : public BaseImage::Image {
        using Image::Image;
    };

    void addPtsToVector(std::vector<Vec2i> &pts, double newX, double newY);
    void DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, std::vector<Vec2i> &linePts);
    void bresenham(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void xiaolinAntiAliasing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void drawRandomLines(int w = BaseImage::width, int h = BaseImage::height, int lineCount = randomLines);
    void drawStarburst(int w = 300, int h = 300, int linesToDraw = 90, int radius = 125);
    void drawWireframe(Model *model, std::string wireFrameName, std::string method = "bresenham");
}


#endif