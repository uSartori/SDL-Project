#include "Rectangle.h"
#include "Line.h"
#include "Transform.h"
#include <Color.h>
#include <cmath>
#include <algorithm>

// Construtor vazio
Rectangle::Rectangle()
{
}

// Destrutor
Rectangle::~Rectangle()
{
}

// Cria um retangulo a partir de um ponto e suas dimensoes
Rectangle::Rectangle(Point xy, double width, double height, Color color)
{
    this->xy = xy;
    this->width = width;
    this->height = height;
    this->color = color;
    this->rotation = 0;
}

// Cria um retangulo usando dois pontos
Rectangle::Rectangle(Point xy0, Point xy1, Color color)
{
    this->xy = xy0;
    this->width = xy1.getX() - xy0.getX();
    this->height = xy1.getY() - xy0.getY();
    this->color = color;
    this->rotation = 0;
}

// Retorna a posição do retangulo
Point Rectangle::getXy()
{
    return xy;
}

// Retorna a largura
double Rectangle::getWidth()
{
    return width;
}

// Retorna a altura
double Rectangle::getHeight()
{
    return height;
}

// Altera a posição
void Rectangle::setXy(Point xy)
{
    this->xy = xy;
}

// Altera a largura
void Rectangle::setWidth(double width)
{
    this->width = width;
}

// Altera a altura
void Rectangle::setHeight(double height)
{
    this->height = height;
}

// Desenha os quatro lados do retangulo
void Rectangle::draw()
{
    Point p1 = xy;
    Point p2(xy.getX() + width, xy.getY());
    Point p3(xy.getX() + width, xy.getY() + height);
    Point p4(xy.getX(), xy.getY() + height);

    Point points[4] = {p1, p2, p3, p4};

    Point center(xy.getX() + width / 2, xy.getY() + height / 2);

    if (rotation != 0)
    {
        Transform transform;
        transform.rotate(points, 4, rotation, center);
    }

    Line line;

    line.drawWuLine(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY(), color);
    line.drawWuLine(points[1].getX(), points[1].getY(), points[2].getX(), points[2].getY(), color);
    line.drawWuLine(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY(), color);
    line.drawWuLine(points[3].getX(), points[3].getY(), points[0].getX(), points[0].getY(), color);
}

// Translada o retangulo
void Rectangle::translate(double tx, double ty)
{
    xy.setX(xy.getX() + tx);
    xy.setY(xy.getY() + ty);
}

// Escala o retangulo usando um ponto de referencia
void Rectangle::scale(double sx, double sy, Point reference)
{
    Point points[4] = {
        Point(xy.getX(), xy.getY()),
        Point(xy.getX() + width, xy.getY()),
        Point(xy.getX() + width, xy.getY() + height),
        Point(xy.getX(), xy.getY() + height)
    };

    Transform transform;
    transform.scale(points, 4, sx, sy, reference);

    int minX = points[0].getX();
    int minY = points[0].getY();
    int maxX = points[0].getX();
    int maxY = points[0].getY();

    for (int i = 1; i < 4; i++)
    {
        minX = std::min(minX, points[i].getX());
        minY = std::min(minY, points[i].getY());
        maxX = std::max(maxX, points[i].getX());
        maxY = std::max(maxY, points[i].getY());
    }

    xy = Point(minX, minY);
    width = maxX - minX;
    height = maxY - minY;
}

// Rotaciona o retangulo
// Rotaciona o retangulo em torno do seu centro
void Rectangle::rotate(double angle, Point reference)
{
    rotation += angle * 0.2;
}

// Verifica se o clique está próximo do retangulo
bool Rectangle::isNear(int clickX, int clickY)
{
    Point p1 = xy;
    Point p2(xy.getX() + width, xy.getY());
    Point p3(xy.getX() + width, xy.getY() + height);
    Point p4(xy.getX(), xy.getY() + height);

    auto nearLine = [clickX, clickY](Point a, Point b)
    {
        double dx = b.getX() - a.getX();
        double dy = b.getY() - a.getY();
        double lengthSquared = dx * dx + dy * dy;

        if (lengthSquared == 0)
        {
            double px = clickX - a.getX();
            double py = clickY - a.getY();
            return px * px + py * py <= 25;
        }

        double t = ((clickX - a.getX()) * dx + (clickY - a.getY()) * dy) / lengthSquared;

        t = std::max(0.0, std::min(1.0, t));

        double closestX = a.getX() + t * dx;
        double closestY = a.getY() + t * dy;
        double distanceX = clickX - closestX;
        double distanceY = clickY - closestY;

        return distanceX * distanceX + distanceY * distanceY <= 25;
    };

    return nearLine(p1, p2) || nearLine(p2, p3) || nearLine(p3, p4) || nearLine(p4, p1);
}

double Rectangle::getRotation() const
{
    return rotation;
}
