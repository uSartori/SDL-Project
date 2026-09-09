#ifndef RECTANGULE_H
#define RECTANGULE_H

#include "Point.h"
#include "Color.h"
#include "Shape.h"

class Rectangle : public Shape
{
public:
    Rectangle();
    Rectangle(Point xy, double width, double height, Color color);
    Rectangle(Point xy0, Point xy1, Color color);

    Point getXy();
    double getWidth();
    double getHeight();

    void setXy(Point xy);
    void setWidth(double width);
    void setHeight(double height);

    void draw() override;
    void translate(double tx, double ty) override;
    void scale(double sx, double sy, Point reference) override;
    void rotate(double angle, Point reference) override;
    bool isNear(int x, int y) override;

    double getRotation() const;

    void drawRectangle();

    virtual ~Rectangle();

private:
    Point points[4];
    Point xy;
    double width;
    double height;
    double rotation;
    Color color;
};

#endif // RECTANGULE_H
