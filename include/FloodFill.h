#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <SDL2/SDL.h>
#include "Point.h"
#include "Color.h"

class FloodFill
{
public:
    FloodFill();
    ~FloodFill();

    Uint32 getPixel(int x, int y);
    void setPixel(int x, int y, Uint32 color);
    bool isSameColor(Uint32 color1, Uint32 color2);
    void fill(Point startPoint, Color newColor);
};

#endif
