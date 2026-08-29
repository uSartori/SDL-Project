#include "Transform.h"
#include "Context.h"
#include "Line.h"

#include <cmath>

Transform::Transform()
{
    // ctor
}

Transform::~Transform(){
    // dtor
}

// Move uma figura de lugar
void Transform::translate(Point points[], int numPoints, double tx, double ty){
    for (int i = 0; i < numPoints; i++)
    {
        points[i].setX(points[i].getX() + tx);
        points[i].setY(points[i].getY() + ty);
    }
}

// Muda o tamanho de uma figura
void Transform::scale(Point points[], int numPoints, double sx, double sy, Point reference){
    // Leva a figura para a origem
    for (int i = 0; i < numPoints; i++)
    {
        points[i].setX(points[i].getX() - reference.getX());
        points[i].setY(points[i].getY() - reference.getY());
    }

    // Aplica a escala
    for (int i = 0; i < numPoints; i++)
    {
        points[i].setX(points[i].getX() * sx);
        points[i].setY(points[i].getY() * sy);
    }

    // Retorna a figura para a posicao original
    for (int i = 0; i < numPoints; i++)
    {
        points[i].setX(points[i].getX() + reference.getX());
        points[i].setY(points[i].getY() + reference.getY());
    }
}

// Rotaciona uma figura
void Transform::rotate(Point points[], int numPoints, double angle, Point reference){
    double rad = angle * M_PI / 180.0;

    double cosA = cos(rad);
    double sinA = sin(rad);

    // Leva a figura para a origem
    for (int i = 0; i < numPoints; i++)
    {
        points[i].setX(points[i].getX() - reference.getX());
        points[i].setY(points[i].getY() - reference.getY());
    }

    // Aplica a rotacao
    for (int i = 0; i < numPoints; i++)
    {
        double x = points[i].getX();
        double y = points[i].getY();

        double newX = x * cosA - y * sinA;
        double newY = x * sinA + y * cosA;

        points[i].setX(newX);
        points[i].setY(newY);
    }

    // Retorna a figura para a posicao original
    for (int i = 0; i < numPoints; i++){
        points[i].setX(points[i].getX() + reference.getX());
        points[i].setY(points[i].getY() + reference.getY());
    }
}
