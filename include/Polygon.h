#ifndef POLYGON_H
#define POLYGON_H

#include "Point.h"
#include "Color.h"

#include <list>

using namespace std;

class Polygon
{
public:
    Polygon();
    Polygon(list<Point> pontos, Color color);

    list<Point> getPontos();
    Color getColor();

    void setPontos(list<Point> pontos);
    void setColor(Color color);

    void drawPolygon(list<Point> pontos, Color cor);

    virtual ~Polygon();

protected:

private:
    list<Point> pontos;
    Color color;
};

#endif // POLYGON_H
