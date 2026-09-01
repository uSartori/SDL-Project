#include "Curve.h"
#include "Line.h"
#include "Transform.h"

#include <cmath>

Curve::Curve()
{
    // ctor
}

Curve::Curve(Point points[], Color color)
{
    for (int i = 0; i < 4; i++)
    {
        this->points[i] = points[i];
    }

    this->color = color;
}

Curve::~Curve()
{
    // dtor
}

Point Curve::getPoint(int index)
{
    return points[index];
}

Color Curve::getColor()
{
    return color;
}

void Curve::setPoint(int index, Point point)
{
    points[index] = point;
}

void Curve::setColor(Color color)
{
    this->color = color;
}

// Desenha a curva de Bezier
void Curve::drawBezierCurve(Point points[], Color color)
{
    float t;
    float x, y;

    Line line;

    for (t = 0.0; t <= 1.0; t += 0.001)
    {
        x = pow(1 - t, 3) * points[0].getX()
          + 3 * pow(1 - t, 2) * t * points[1].getX()
          + 3 * (1 - t) * pow(t, 2) * points[2].getX()
          + pow(t, 3) * points[3].getX();

        y = pow(1 - t, 3) * points[0].getY()
          + 3 * pow(1 - t, 2) * t * points[1].getY()
          + 3 * (1 - t) * pow(t, 2) * points[2].getY()
          + pow(t, 3) * points[3].getY();

        line.setPixel(
            (int)x,
            (int)y,
            color
        );
    }
}

// Função padrao do Shape
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
void Curve::scale(double sx, double sy)
{
    Transform transform;

    Point reference = points[0];

    transform.scale(points, 4, sx, sy, reference);
}

// Rotaciona a curva
void Curve::rotate(double angle)
{
    Transform transform;

    Point reference = points[0];

    transform.rotate(points, 4, angle, reference);
}

bool Curve::isNear(int clickX, int clickY)
{
    for (float t = 0.0f; t <= 1.0f; t += 0.001f)
    {
        float x =
            pow(1 - t, 3) * points[0].getX()
            + 3 * pow(1 - t, 2) * t * points[1].getX()
            + 3 * (1 - t) * pow(t, 2) * points[2].getX()
            + pow(t, 3) * points[3].getX();

        float y =
            pow(1 - t, 3) * points[0].getY()
            + 3 * pow(1 - t, 2) * t * points[1].getY()
            + 3 * (1 - t) * pow(t, 2) * points[2].getY()
            + pow(t, 3) * points[3].getY();

        int pixelX = (int)x;
        int pixelY = (int)y;

        int dx = clickX - pixelX;
        int dy = clickY - pixelY;

        // Distancia euclidiana ao quadrado.
        // 5 pixels -> 25.
        if (dx * dx + dy * dy <= 25)
        {
            return true;
        }
    }

    return false;
}
