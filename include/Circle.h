#ifndef CIRCLE_H
#define CIRCLE_H

#include "Point.h"
#include "Color.h"

class Circle
{
public:
    Circle();
    Circle(Point xy, double radius, Color color);

    Point getXy();
    double getRadius();

    void setColor(Color cor);
    void setRadius(double radius);
    void setXy(Point xy);

    void drawBresenhamCircle(Point xy, double radius, Color cor);

    virtual ~Circle();

protected:

private:
    Point xy;
    double radius;
    Color color;

    void displayBresenhamCircle(Point xy, int x, int y, Color cor);
};

#endif // CIRCLE_H
