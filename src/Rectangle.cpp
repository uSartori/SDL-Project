#include "Rectangle.h"
#include "Line.h"
#include "Transform.h"
#include <Color.h>
#include <cmath>

Rectangle::Rectangle()
{
    // ctor
}

Rectangle::~Rectangle()
{
    // dtor
}

Rectangle::Rectangle(Point xy, double width, double height, Color color)
{
    this->xy = xy;
    this->width = width;
    this->height = height;
    this->color = color;
}

Rectangle::Rectangle(Point xy0, Point xy1, Color color)
{
    this->xy = xy0;
    this->width = xy1.getX() - xy0.getX();
    this->height = xy1.getY() - xy0.getY();
    this->color = color;
}

Point Rectangle::getXy()
{
    return xy;
}

double Rectangle::getWidth()
{
    return width;
}

double Rectangle::getHeight()
{
    return height;
}

void Rectangle::setXy(Point xy)
{
    this->xy = xy;
}

void Rectangle::setWidth(double width)
{
    this->width = width;
}

void Rectangle::setHeight(double height)
{
    this->height = height;
}

// Desenha os quatro lados do retangulo
void Rectangle::draw()
{
    Point p1 = xy;
    Point p2 = Point(xy.getX() + width, xy.getY());
    Point p3 = Point(xy.getX() + width, xy.getY() + height);
    Point p4 = Point(xy.getX(), xy.getY() + height);

    Line line;

    line.drawWuLine(
        p1.getX(), p1.getY(),
        p2.getX(), p2.getY(),
        color
    );

    line.drawWuLine(
        p2.getX(), p2.getY(),
        p3.getX(), p3.getY(),
        color
    );

    line.drawWuLine(
        p3.getX(), p3.getY(),
        p4.getX(), p4.getY(),
        color
    );

    line.drawWuLine(
        p4.getX(), p4.getY(),
        p1.getX(), p1.getY(),
        color
    );
}

// Translada o retangulo
void Rectangle::translate(double tx, double ty)
{
    Transform transform;

    xy.setX(xy.getX() + tx);
    xy.setY(xy.getY() + ty);
}

// Escala o retangulo usando seu ponto inicial como referencia
void Rectangle::scale(double sx, double sy)
{
    Transform transform;

    width = width * sx;
    height = height * sy;
}

// Rotaciona o retangulo
void Rectangle::rotate(double angle)
{
    Transform transform;

    Point p1 = xy;
    Point p2 = Point(xy.getX() + width, xy.getY());
    Point p3 = Point(xy.getX() + width, xy.getY() + height);
    Point p4 = Point(xy.getX(), xy.getY() + height);

    Point points[4] = {p1, p2, p3, p4};

    transform.rotate(points, 4, angle, xy);

    // Atualiza a posição e as dimensões do retângulo
    xy = points[0];

    width = sqrt(
                pow(points[1].getX() - points[0].getX(), 2) +
                pow(points[1].getY() - points[0].getY(), 2)
            );

    height = sqrt(
                 pow(points[3].getX() - points[0].getX(), 2) +
                 pow(points[3].getY() - points[0].getY(), 2)
             );
}
bool Rectangle::isNear(int clickX, int clickY)
{
    Point p1 = xy;
    Point p2 = Point(xy.getX() + width, xy.getY());
    Point p3 = Point(xy.getX() + width, xy.getY() + height);
    Point p4 = Point(xy.getX(), xy.getY() + height);

    // Verifica a distancia para cada lado
    Line line;

    // Funcao auxiliar para calcular a distancia
    // do clique ao segmento.
    auto nearLine =
        [clickX, clickY](Point a, Point b)
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

        double t =
            ((clickX - a.getX()) * dx +
             (clickY - a.getY()) * dy) /
            lengthSquared;

        if (t < 0)
        {
            t = 0;
        }

        if (t > 1)
        {
            t = 1;
        }

        double closestX = a.getX() + t * dx;
        double closestY = a.getY() + t * dy;

        double distanceX = clickX - closestX;
        double distanceY = clickY - closestY;

        return distanceX * distanceX +
               distanceY * distanceY <= 25;
    };

    return nearLine(p1, p2) ||
           nearLine(p2, p3) ||
           nearLine(p3, p4) ||
           nearLine(p4, p1);
}
