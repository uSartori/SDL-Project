#ifndef LINE_H
#define LINE_H
#include <Point.h>
#include <Color.h>
#include<SDL2/SDL.h>

#include "Shape.h"

class Line : public Shape
{
    public:
        Line();
        Line(Point start, Point end, Color color);
        Line(Point start, Point end, Color color, int antialias);

        void setPixel(int x, int y, int r, int g, int b);
        void setPixel(int x, int y, Color color);

        Point getStart();
        Point getEnd();

        void setStart(Point start);
        void setEnd(Point end);

        void draw() override;
        void translate(double tx, double ty) override;
        void scale( double sx, double sy, Point reference) override;
        void rotate(double angle, Point reference) override;
        bool isNear(int x, int y) override;

        void drawWuLine(int x0, int y0, int x1, int y1, Color color);
        void bresenham(int x1, int y1, int x2, int y2, int r, int g, int b);

        virtual ~Line();

    protected:

    private:
        Point start;
        Point end;
        Color color;
        int antialias = 0;
        void setPixel(int x, int y, int r, int g, int b, int a);
        void setPixel(int x, int y, Uint32 cor);
        Uint32 getPixel(int x, int y);

};

#endif // LINE_H
