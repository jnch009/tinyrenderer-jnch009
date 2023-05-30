#ifndef __BASE_H__
#define __BASE_H__

#include "color.h"
#include "tgaimage.h"

namespace BaseImage {
    inline int width = 800;
    inline int height = 800;
    inline int bpp = TGAImage::RGB;

    class Image {
    public:
        int width;
        int height;
        Color color;
        TGAImage *image;
        Image() {
            width = BaseImage::width;
            height = BaseImage::height;
            image = new TGAImage(width, height, bpp);
        };

        Image(int w, int h) {
            width = w;
            height = h;
            image = new TGAImage(width, height, bpp);
        };
    };
}

#endif