#include "Canvas.h"
#include "Context.h"
#include "Line.h"

Canvas::Canvas(int width, int height)
{
    this->width = width;
    this->height = height;
}

Canvas::~Canvas()
{
    for (Shape* shape : shapes)
    {
        delete shape;
    }

    shapes.clear();
}

void Canvas::addShape(Shape* shape)
{
    if (shape != nullptr)
    {
        shapes.push_back(shape);
    }
}

void Canvas::draw()
{
    for (Shape* shape : shapes)
    {
        if (shape != nullptr)
        {
            shape->draw();
        }
    }
}

void Canvas::clear()
{
    Line line;

    SDL_Surface* window_surface =
        Context::getInstance()->getWindowSurface();

    for (int x = 0; x < window_surface->w; x++)
    {
        for (int y = 0; y < window_surface->h; y++)
        {
            line.setPixel(x, y, 255, 255, 255);
        }
    }
}

std::vector<Shape*>& Canvas::getShapes()
{
    return shapes;
}
