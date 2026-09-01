#include "Circle.h"
#include "Line.h"

#include <cmath>

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

// Desenha os 8 pontos simetricos do circulo
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

// Algoritmo de Bresenham para desenhar o circulo
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

// Desenha o circulo
void Circle::draw()
{
    drawBresenhamCircle(xy, radius, color);
}

// Translada o circulo
void Circle::translate(double tx, double ty)
{
    xy.setX(xy.getX() + tx);
    xy.setY(xy.getY() + ty);
}

// Escala o circulo
void Circle::scale(double sx, double sy)
{
    radius = radius * sx;
}

// Rotaciona o circulo
void Circle::rotate(double angle)
{
    // A rotacao nao afeta o circulo
}

bool Circle::isNear(int clickX, int clickY)
{
    double dx = clickX - xy.getX();
    double dy = clickY - xy.getY();

    double distanceSquared =
        dx * dx + dy * dy;

    double radiusDifference =
        sqrt(distanceSquared) - radius;

    // Considera uma faixa de 5 pixels em torno
    // dos pixels que formam a circunferencia.
    return fabs(radiusDifference) <= 5.0;
}
