#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <SDL2/SDL.h>
#include "Color.h"
#include "Point.h"

class FloodFill
{
public:
    FloodFill();
    ~FloodFill();

    void fill(Point startPoint, Color newColor);

private:
    Uint32 getPixel(int x, int y);
    void setPixel(int x, int y, Color color);

    bool isSameColor(Uint32 color1, Uint32 color2);
};

#endif // FLOODFILL_H
