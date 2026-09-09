#include "Curve.h"
#include "Line.h"
#include "Transform.h"

#include <algorithm>
#include <cmath>

// Construtor vazio
Curve::Curve()
{
}

// Cria uma curva com 4 pontos
Curve::Curve(Point points[], Color color)
{
    for (int i = 0; i < 4; i++)
        this->points[i] = points[i];

    this->color = color;
}

// Destrutor
Curve::~Curve()
{
}

// Retorna um ponto
Point Curve::getPoint(int index)
{
    return points[index];
}

// Retorna a cor
Color Curve::getColor()
{
    return color;
}

// Altera um ponto
void Curve::setPoint(int index, Point point)
{
    points[index] = point;
}

// Altera a cor
void Curve::setColor(Color color)
{
    this->color = color;
}

// Desenha a curva de Bezier
void Curve::drawBezierCurve(Point points[], Color color)
{
    Line line;

    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        double x =
            pow(1 - t, 3) * points[0].getX() +
            3 * pow(1 - t, 2) * t * points[1].getX() +
            3 * (1 - t) * pow(t, 2) * points[2].getX() +
            pow(t, 3) * points[3].getX();

        double y =
            pow(1 - t, 3) * points[0].getY() +
            3 * pow(1 - t, 2) * t * points[1].getY() +
            3 * (1 - t) * pow(t, 2) * points[2].getY() +
            pow(t, 3) * points[3].getY();

        line.setPixel((int)x, (int)y, color);
    }
}

// Desenha a curva
void Curve::draw()
{
    drawBezierCurve(points, color);
}

// Translada a curva
void Curve::translate(double tx, double ty)
{
    Transform transform;
    transform.translate(points, 4, tx, ty);
}

// Escala a curva
void Curve::scale(double sx, double sy, Point reference)
{
    Transform transform;
    transform.scale(points, 4, sx, sy, reference);
}

// Rotaciona a curva usando o centro da figura como referencia
void Curve::rotate(double angle, Point reference)
{
    Transform transform;
    transform.rotate(points, 4, angle, reference);
}

// Verifica se o clique está perto da curva
bool Curve::isNear(int clickX, int clickY)
{
    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        double x =
            pow(1 - t, 3) * points[0].getX() +
            3 * pow(1 - t, 2) * t * points[1].getX() +
            3 * (1 - t) * pow(t, 2) * points[2].getX() +
            pow(t, 3) * points[3].getX();

        double y =
            pow(1 - t, 3) * points[0].getY() +
            3 * pow(1 - t, 2) * t * points[1].getY() +
            3 * (1 - t) * pow(t, 2) * points[2].getY() +
            pow(t, 3) * points[3].getY();

        int dx = clickX - (int)x;
        int dy = clickY - (int)y;

        if (dx * dx + dy * dy <= 25)
            return true;
    }

    return false;
}
