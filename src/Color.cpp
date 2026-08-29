#include "Color.h"
#include "Context.h"

Color::Color()
{
    //ctor
}


Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

Color::Color(Uint8 r, Uint8 g, Uint8 b)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = 255;
}

Color::~Color()
{
    //dtor
}

Uint32 Color::getColor(int r, int g, int b) {
    return RGB(r, g, b);
}

Uint32 Color::getColor() {
    return RGB(this->r, this->g, this->b);
}

Uint8 Color::getR()
{
    return this->r;
}
Uint8 Color::getG()
{
    return this->g;
}
Uint8 Color::getB()
{
    return this->b;
}

Uint32 Color::RGB(int r, int g, int b) {

    SDL_Surface * surface = Context::getInstance()->getWindowSurface();

    return SDL_MapRGBA(surface->format, r, g, b, 255);
}

Uint8 Color::getColorComponent( Uint32 pixel, char component ) {

    Uint32 mask;

    switch(component) {
        case 'b' :
        case 'B' :
            mask = RGB(0,0,255);
            pixel = pixel & mask;
            break;
        case 'r' :
        case 'R' :
            mask = RGB(255,0,0);
            pixel = pixel & mask;
            pixel = pixel >> 16;
            break;
        case 'g' :
        case 'G' :
            mask = RGB(0,255,0);
            pixel = pixel & mask;
            pixel = pixel >> 8;
            break;
    }
    return (Uint8) pixel;
}
