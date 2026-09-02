#include "Transform.h"

#include <cmath>

// Construtor
Transform::Transform()
{
}

// Destrutor
Transform::~Transform()
{
}

// Move uma figura
void Transform::translate(Point points[], int numPoints, double tx, double ty)
{
    for (int i = 0; i < numPoints; i++)
    {
        points[i].setX(points[i].getX() + tx);
        points[i].setY(points[i].getY() + ty);
    }
}

// Altera o tamanho de uma figura
void Transform::scale(Point points[], int numPoints, double sx, double sy, Point reference)
{
    for (int i = 0; i < numPoints; i++)
    {
        double x = points[i].getX() - reference.getX();
        double y = points[i].getY() - reference.getY();

        points[i].setX(x * sx + reference.getX());
        points[i].setY(y * sy + reference.getY());
    }
}

// Rotaciona uma figura
void Transform::rotate(Point points[], int numPoints, double angle, Point reference)
{
    double rad = angle * M_PI / 180.0;
    double cosA = cos(rad);
    double sinA = sin(rad);

    for (int i = 0; i < numPoints; i++)
    {
        double x = points[i].getX() - reference.getX();
        double y = points[i].getY() - reference.getY();

        double newX = x * cosA - y * sinA + reference.getX();
        double newY = x * sinA + y * cosA + reference.getY();

        points[i].setX(newX);
        points[i].setY(newY);
    }
}
