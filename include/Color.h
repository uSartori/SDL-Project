#ifndef COLOR_H
#define COLOR_H
#include<SDL2/SDL.h>


class Color
{
    public:
        Color();
        Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        Color(Uint8 r, Uint8 g, Uint8 b);
        Uint8 getColorComponent( Uint32 pixel, char component );
        static Uint32 RGB(int r, int g, int b);
        Uint32 getColor();
        Uint32 getColor(int r, int g, int b);
        Uint8 getR();
        Uint8 getG();
        Uint8 getB();
        virtual ~Color();

    protected:

    private:
        Uint8 r, g, b, a;
};

#endif // COLOR_H
