#ifndef CURVE_H
#define CURVE_H

#include <Point.h>
#include <Color.h>

class Curve
{
public:
    Curve();
    Curve(Point p0, Point p1, Point p2, Point p3, Color color);

    Point getP0();
    Point getP1();
    Point getP2();
    Point getP3();

    Color getColor();

    void setP0(Point p0);
    void setP1(Point p1);
    void setP2(Point p2);
    void setP3(Point p3);

    void drawBezierCurve(Point p0, Point p1, Point p2, Point p3, Color color);

    virtual ~Curve();

private:
    Point p0;
    Point p1;
    Point p2;
    Point p3;
    Color color;
};

#endif // CURVE_H
