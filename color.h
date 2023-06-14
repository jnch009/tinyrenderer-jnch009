#ifndef __COLOR_H__
#define __COLOR_H__

#include "tgaimage.h"

struct Color {
    const TGAColor white = TGAColor(255, 255, 255, 255);
    const TGAColor red = TGAColor(255, 0,   0,   255);
    const TGAColor blue = TGAColor(0, 0,   255,   255);
    const TGAColor green = TGAColor(0, 255,  0,   255);
    const TGAColor yellow = TGAColor(255,255,0,255);
};

#endif