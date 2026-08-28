#include "Circle.h"
#include "Context.h"
#include "Line.h"

Circle::Circle()
{
    // ctor
}

Circle::~Circle()
{
    // dtor
}

Circle::Circle(Point xy, double radius, Color cor)
{
    this->xy = xy;
    this->radius = radius;
    this->color = cor;
}

Point Circle::getXy()
{
    return xy;
}

double Circle::getRadius()
{
    return radius;
}

void Circle::setRadius(double radius)
{
    this->radius = radius;
}

void Circle::setXy(Point xy)
{
    this->xy = xy;
}

void Circle::setColor(Color cor)
{
    this->color = cor;
}

void Circle::displayBresenhamCircle(Point xy, int x, int y, Color cor)
{
    Line line;

    int xc = xy.getX();
    int yc = xy.getY();

    line.setPixel(xc + x, yc + y, cor);
    line.setPixel(xc - x, yc + y, cor);
    line.setPixel(xc + x, yc - y, cor);
    line.setPixel(xc - x, yc - y, cor);
    line.setPixel(xc + y, yc + x, cor);
    line.setPixel(xc - y, yc + x, cor);
    line.setPixel(xc + y, yc - x, cor);
    line.setPixel(xc - y, yc - x, cor);
}

void Circle::drawBresenhamCircle(Point xy, double radius, Color cor)
{
    int x = 0;
    int y = radius;

    int decisionParameter = 3 - 2 * radius;

    displayBresenhamCircle(xy, x, y, cor);

    while (y >= x)
    {
        x++;

        if (decisionParameter > 0)
        {
            y--;
            decisionParameter = decisionParameter + 4 * (x - y) + 10;
        }
        else
        {
            decisionParameter = decisionParameter + 4 * x + 6;
        }

        displayBresenhamCircle(xy, x, y, cor);
    }
}
