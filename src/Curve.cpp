#include "Curve.h"
#include "Context.h"
#include "Line.h"

#include <cmath>

Curve::Curve()
{
    // ctor
}

Curve::Curve(Point p0, Point p1, Point p2, Point p3, Color color)
{
    this->p0 = p0;
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->color = color;
}

Curve::~Curve()
{
    // dtor
}

Point Curve::getP0()
{
    return p0;
}

Point Curve::getP1()
{
    return p1;
}

Point Curve::getP2()
{
    return p2;
}

Point Curve::getP3()
{
    return p3;
}

Color Curve::getColor()
{
    return color;
}

void Curve::setP0(Point p0)
{
    this->p0 = p0;
}

void Curve::setP1(Point p1)
{
    this->p1 = p1;
}

void Curve::setP2(Point p2)
{
    this->p2 = p2;
}

void Curve::setP3(Point p3)
{
    this->p3 = p3;
}

void Curve::drawBezierCurve(Point p0, Point p1, Point p2, Point p3, Color color)
{
    float t;
    float x,y;
    Line line;

    for (t = 0.0; t <= 1.0; t += 0.001)
    {
        x = pow(1 - t, 3) * p0.getX()
          + 3 * pow(1 - t, 2) * t * p1.getX()
          + 3 * (1 - t) * pow(t, 2) * p2.getX()
          + pow(t, 3) * p3.getX();

        y = pow(1 - t, 3) * p0.getY()
          + 3 * pow(1 - t, 2) * t * p1.getY()
          + 3 * (1 - t) * pow(t, 2) * p2.getY()
          + pow(t, 3) * p3.getY();

        line.setPixel((int)x, (int)y,
                 color.getR(),
                 color.getG(),
                 color.getB());
    }
}
