#ifndef CURVE_H
#define CURVE_H

#include <Point.h>
#include <Color.h>
#include "Shape.h"

class Curve : public Shape
{
public:
    Curve();
    Curve(Point points[], Color color);

    Point getPoint(int index);
    Color getColor();

    void setPoint(int index, Point point);
    void setColor(Color color);

    void draw();

    void translate(double tx, double ty);
    void scale(double sx, double sy);
    void rotate(double angle);

    virtual ~Curve();

private:
    Point points[4];
    Color color;

    void drawBezierCurve(Point points[], Color color);
};

#endif // CURVE_H
