#include "Circle.h"
#include "Line.h"
#include "Transform.h"
#include <cmath>

// Destrutor
Circle::~Circle()
{
}

// Cria um circulo
Circle::Circle(Point xy, double radius, Color color)
{
    this->xy = xy;
    this->radius = radius;
    this->color = color;
}

// Retorna a posição
Point Circle::getXy()
{
    return xy;
}

// Retorna o raio
double Circle::getRadius()
{
    return radius;
}

// Altera o raio
void Circle::setRadius(double radius)
{
    this->radius = radius;
}

// Altera a posição
void Circle::setXy(Point xy)
{
    this->xy = xy;
}

// Altera a cor
void Circle::setColor(Color color)
{
    this->color = color;
}

// Desenha os 8 pontos simetricos
void Circle::displayBresenhamCircle(Point xy, int x, int y, Color color)
{
    Line line;

    int xc = xy.getX();
    int yc = xy.getY();

    line.setPixel(xc + x, yc + y, color);
    line.setPixel(xc - x, yc + y, color);
    line.setPixel(xc + x, yc - y, color);
    line.setPixel(xc - x, yc - y, color);

    line.setPixel(xc + y, yc + x, color);
    line.setPixel(xc - y, yc + x, color);
    line.setPixel(xc + y, yc - x, color);
    line.setPixel(xc - y, yc - x, color);
}

// Algoritmo de Bresenham
void Circle::drawBresenhamCircle(Point xy, double radius, Color color)
{
    int x = 0;
    int y = radius;
    int decisionParameter = 3 - 2 * radius;

    displayBresenhamCircle(xy, x, y, color);

    while (y >= x)
    {
        x++;

        if (decisionParameter > 0)
        {
            y--;
            decisionParameter += 4 * (x - y) + 10;
        }
        else
        {
            decisionParameter += 4 * x + 6;
        }

        displayBresenhamCircle(xy, x, y, color);
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
    radius *= sx;
}

// Rotacao nao altera o circulo
void Circle::rotate(double angle)
{
}

// Verifica se o clique está perto do circulo
bool Circle::isNear(int clickX, int clickY)
{
    double dx = clickX - xy.getX();
    double dy = clickY - xy.getY();

    double distance = sqrt(dx * dx + dy * dy);

    return fabs(distance - radius) <= 5.0;
}

// Escala o circulo usando um ponto de referencia
void Circle::scaleFromMouse(double sx, double sy, Point reference)
{
    double scaleFactor = (sx + sy) / 2.0;

    Point point = xy;

    Transform transform;
    transform.scale(&point, 1, scaleFactor, scaleFactor, reference);

    xy = point;
    radius *= scaleFactor;
}
