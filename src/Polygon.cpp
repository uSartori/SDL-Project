#include "Polygon.h"
#include "Context.h"
#include "Line.h"
#include "Transform.h"
#include <algorithm>

// Construtor vazio
Polygon::Polygon()
{
}

// Destrutor
Polygon::~Polygon()
{
}

// Cria um poligono
Polygon::Polygon(list<Point> pontos, Color color)
{
    this->pontos = pontos;
    this->color = color;
}

// Retorna os pontos
list<Point> Polygon::getPontos()
{
    return pontos;
}

// Retorna a cor
Color Polygon::getColor()
{
    return color;
}

// Altera os pontos
void Polygon::setPontos(list<Point> pontos)
{
    this->pontos = pontos;
}

// Altera a cor
void Polygon::setColor(Color color)
{
    this->color = color;
}

// Desenha o poligono ligando os pontos
void Polygon::drawPolygon(list<Point> pontos, Color cor)
{
    if (pontos.size() < 2)
        return;

    Point primeiro = pontos.front();
    Point anterior = primeiro;
    Point atual;

    int i = 0;

    for (Point ponto : pontos)
    {
        if (i > 0)
        {
            atual = ponto;

            Line line;
            line.drawWuLine(anterior.getX(), anterior.getY(), atual.getX(), atual.getY(), cor);

            anterior = atual;
        }

        i++;
    }

    Line line;
    line.drawWuLine(atual.getX(), atual.getY(), primeiro.getX(), primeiro.getY(), cor);
}

// Desenha o poligono
void Polygon::draw()
{
    drawPolygon(pontos, color);
}

// Translada os pontos do poligono
void Polygon::translate(double tx, double ty)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();
    Point* points = new Point[numPoints];

    int i = 0;

    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    transform.translate(points, numPoints, tx, ty);

    pontos.clear();

    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    delete[] points;
}

// Escala o poligono usando o primeiro ponto como referencia
void Polygon::scale(double sx, double sy)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();
    Point* points = new Point[numPoints];

    int i = 0;

    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    transform.scale(points, numPoints, sx, sy, points[0]);

    pontos.clear();

    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    delete[] points;
}

void Polygon::rotate(double angle, Point reference)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();

    Point* points = new Point[numPoints];

    int i = 0;

    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    transform.rotate(points, numPoints, angle, reference);

    pontos.clear();

    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    delete[] points;
}

// Verifica se o clique está perto do poligono
bool Polygon::isNear(int clickX, int clickY)
{
    if (pontos.size() < 2)
        return false;

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

    Point first = pontos.front();
    Point previous = first;

    for (auto it = std::next(pontos.begin()); it != pontos.end(); ++it)
    {
        if (nearLine(previous, *it))
            return true;

        previous = *it;
    }

    return nearLine(previous, first);
}

// Escala o poligono usando um ponto de referencia
void Polygon::scaleFromMouse(double sx, double sy, Point reference)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();
    Point* points = new Point[numPoints];

    int i = 0;

    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    transform.scale(points, numPoints, sx, sy, reference);

    pontos.clear();

    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    delete[] points;
}
