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

bool Polygon::isNear(int clickX, int clickY)
{
    if (pontos.size() < 2)
    {
        return false;
    }

    auto it1 = pontos.begin();
    auto it2 = std::next(it1);

    while (it2 != pontos.end())
    {
        Point a = *it1;
        Point b = *it2;

        double dx = b.getX() - a.getX();
        double dy = b.getY() - a.getY();

        double lengthSquared = dx * dx + dy * dy;

        if (lengthSquared == 0)
        {
            double px = clickX - a.getX();
            double py = clickY - a.getY();

            if (px * px + py * py <= 25)
            {
                return true;
            }
        }
        else
        {
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

            if (distanceX * distanceX +
                    distanceY * distanceY <= 25)
            {
                return true;
            }
        }

        ++it1;
        ++it2;
    }

    // Verifica também o ultimo lado:
    // ultimo ponto -> primeiro ponto
    Point first = pontos.front();
    Point last = pontos.back();

    double dx = first.getX() - last.getX();
    double dy = first.getY() - last.getY();

    double lengthSquared = dx * dx + dy * dy;

    if (lengthSquared == 0)
    {
        double px = clickX - last.getX();
        double py = clickY - last.getY();

        return px * px + py * py <= 25;
    }

    double t =
        ((clickX - last.getX()) * dx +
         (clickY - last.getY()) * dy) /
        lengthSquared;

    if (t < 0)
    {
        t = 0;
    }

    if (t > 1)
    {
        t = 1;
    }

    double closestX = last.getX() + t * dx;
    double closestY = last.getY() + t * dy;

    double distanceX = clickX - closestX;
    double distanceY = clickY - closestY;

    return distanceX * distanceX +
           distanceY * distanceY <= 25;
}
