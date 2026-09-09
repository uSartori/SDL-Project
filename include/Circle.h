#ifndef CIRCLE_H
#define CIRCLE_H

#include "Point.h"
#include "Color.h"
#include "Shape.h"

class Circle : public Shape
{
public:
    Circle();
    Circle(Point xy, double radius, Color color);

    Point getXy();
    double getRadius();

    void setColor(Color cor);
    void setRadius(double radius);
    void setXy(Point xy);

    void draw() override;
    void translate(double tx, double ty) override;
    void scale(double sx, double sy, Point reference) override;
    void rotate(double angle, Point reference) override;
    bool isNear(int x, int y) override;


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
