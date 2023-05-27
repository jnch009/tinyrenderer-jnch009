#ifndef __LINE_H__
#define __LINE_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"
#include "color.h"

namespace Line {
    inline int width = 800;
    inline int height = 800;
    inline int bpp = TGAImage::RGB;
    inline int randomLines = 25;

    class LineImage {
        public:
            int width;
            int height;
            Color color;
            TGAImage *lineImage;
            LineImage() {
                lineImage = new TGAImage(Line::width, Line::height, Line::bpp);
            };

            LineImage(int w, int h) {
                lineImage = new TGAImage(w, h, bpp);
                width = w;
                height = h;
            }
    };

    void addPtsToVector(std::vector<Vec2i> &pts, double newX, double newY);
    void DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void DDA(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, std::vector<Vec2i> &linePts);
    void bresenham(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void xiaolinAntiAliasing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    void drawRandomLines(int w = Line::width, int h = Line::height, int lineCount = Line::randomLines);
    void drawStarburst();
}


#endif