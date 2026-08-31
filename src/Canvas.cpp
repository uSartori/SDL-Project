#include "Canvas.h"
#include "Context.h"
#include "Line.h"
#include "FloodFill.h"

Canvas::Canvas(int width, int height)
{
    this->width = width;
    this->height = height;

    // O Canvas começa abaixo da Toolbar
    this->x = 0;
    this->y = 40;
}

Canvas::~Canvas()
{
    for (Shape* shape : shapes)
    {
        delete shape;
    }
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
    // Primeiro desenha as figuras
    for (Shape* shape : shapes)
    {
        if (shape != nullptr)
        {
            shape->draw();
        }
    }

    // Depois reaplica os Flood Fills
    FloodFill floodFill;

    for (const Fill& fill : fills)
    {
        floodFill.fill(
            fill.point,
            fill.color
        );
    }
}

void Canvas::clear()
{
    Context* context = Context::getInstance();

    // Define o viewport do Canvas
    context->setViewport(
        x,
        y,
        width,
        height
    );

    Line line;

    // Limpa somente a área do Canvas
    for (int px = x; px < x + width; px++)
    {
        for (int py = y; py < y + height; py++)
        {
            line.setPixel(
                px,
                py,
                255,
                255,
                255
            );
        }
    }
}

// Verifica se um ponto pertence à área do Canvas
bool Canvas::isInside(int px, int py) const
{
    return px >= x &&
           px < x + width &&
           py >= y &&
           py < y + height;
}

std::vector<Shape*>& Canvas::getShapes()
{
    return shapes;
}

void Canvas::addFloodFill(Point startPoint, Color newColor)
{
    // Verifica se o clique está dentro do Canvas
    if (!isInside(
            startPoint.getX(),
            startPoint.getY()))
    {
        return;
    }

    // Guarda o Flood Fill
    Fill fill;

    fill.point = startPoint;
    fill.color = newColor;

    fills.push_back(fill);
}
