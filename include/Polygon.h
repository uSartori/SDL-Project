#ifndef POLYGON_H
#define POLYGON_H

#include "Point.h"
#include "Color.h"
#include "Shape.h"

#include <list>

using namespace std;

class Polygon : public Shape
{
public:
    Polygon();
    Polygon(list<Point> pontos, Color color);

    list<Point> getPontos();
    Color getColor();

    void setPontos(list<Point> pontos);
    void setColor(Color color);

    void draw() override;
    void translate(double tx, double ty) override;
    void scale(double sx, double sy) override;
    void rotate(double angle) override;
    bool isNear(int x, int y) override;
    void scaleFromMouse( double sx, double sy, Point reference) override;

    void drawPolygon(list<Point> pontos, Color cor);

    virtual ~Polygon();

protected:

private:
    list<Point> pontos;
    Color color;
};

#endif // POLYGON_H
