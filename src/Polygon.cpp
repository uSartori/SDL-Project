#include "Polygon.h"
#include "Context.h"
#include "Line.h"
#include "Transform.h"

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

// Desenha o poligono ligando cada ponto ao proximo
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
// Desenha o poligono
void Polygon::draw()
{
    drawPolygon(pontos, color);
}

// Translada todos os pontos do poligono
void Polygon::translate(double tx, double ty)
{
    Transform transform;

    int numPoints = pontos.size();

    if (numPoints == 0)
    {
        return;
    }

    Point points[numPoints];

    int i = 0;

    for (Point p : pontos)
    {
        points[i] = p;
        i++;
    }

    transform.translate(points, numPoints, tx, ty);

    pontos.clear();

    for (int i = 0; i < numPoints; i++)
    {
        pontos.push_back(points[i]);
    }
}

// Altera o tamanho do poligono usando o primeiro ponto como referencia
void Polygon::scale(double sx, double sy)
{
    Transform transform;

    int numPoints = pontos.size();

    if (numPoints == 0)
    {
        return;
    }

    Point points[numPoints];

    int i = 0;

    for (Point p : pontos)
    {
        points[i] = p;
        i++;
    }

    Point reference = points[0];

    transform.scale(
        points,
        numPoints,
        sx,
        sy,
        reference
    );

    pontos.clear();

    for (int i = 0; i < numPoints; i++)
    {
        pontos.push_back(points[i]);
    }
}

// Rotaciona o poligono usando o primeiro ponto como referencia
void Polygon::rotate(double angle)
{
    Transform transform;

    int numPoints = pontos.size();

    if (numPoints == 0)
    {
        return;
    }

    Point points[numPoints];

    int i = 0;

    for (Point p : pontos)
    {
        points[i] = p;
        i++;
    }

    Point reference = points[0];

    transform.rotate(
        points,
        numPoints,
        angle,
        reference
    );

    pontos.clear();

    for (int i = 0; i < numPoints; i++)
    {
        pontos.push_back(points[i]);
    }
}
