#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <SDL2/SDL.h>
#include "Shape.h"

class Canvas
{
public:
    Canvas(int width, int height);
    ~Canvas();

    void addShape(Shape* shape);
    void draw();
    void clear();

    std::vector<Shape*>& getShapes();

private:
    int width;
    int height;

    std::vector<Shape*> shapes;
};

#endif // CANVAS_H
