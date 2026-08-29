#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Point.h"

class Transform
{
public:
    Transform();

    void translate(Point points[], int numPoints, double tx, double ty);

    void scale(Point points[], int numPoints, double sx, double sy, Point reference);

    void rotate(Point points[], int numPoints, double angle, Point reference);

    virtual ~Transform();

private:
};

#endif // TRANSFORM_H
