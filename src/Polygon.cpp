#include "Polygon.h"
#include "Context.h"
#include "Line.h"

Polygon::Polygon()
{
    // ctor
}

Polygon::~Polygon()
{
    // dtor
}

Polygon::Polygon(list<Point> pontos, Color color)
{
    this->pontos = pontos;
    this->color = color;
}

list<Point> Polygon::getPontos()
{
    return pontos;
}

Color Polygon::getColor()
{
    return color;
}

void Polygon::setPontos(list<Point> pontos)
{
    this->pontos = pontos;
}

void Polygon::setColor(Color color)
{
    this->color = color;
}

void Polygon::drawPolygon(list<Point> pontos, Color cor)
{
    if (pontos.size() < 2)
    {
        return;
    }

    Point primeiro = pontos.front();
    Point anterior = pontos.front();

    Point atual;

    int i = 0;

    for (Point p : pontos)
    {
        if (i > 0)
        {
            atual = p;

            Line line;

            line.drawWuLine(
                anterior.getX(),
                anterior.getY(),
                atual.getX(),
                atual.getY(),
                cor
            );

            anterior = atual;
        }

        i++;
    }

    Line line;

    line.drawWuLine(
        atual.getX(),
        atual.getY(),
        primeiro.getX(),
        primeiro.getY(),
        cor
    );
}
