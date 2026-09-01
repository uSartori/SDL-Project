#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <SDL2/SDL.h>

#include "Shape.h"
#include "Point.h"
#include "Color.h"

class Canvas
{
public:
    Canvas(int width, int height);
    ~Canvas();

    void addShape(Shape* shape);
    void addFloodFill(Point startPoint, Color newColor);

    void draw();
    void clear();

    bool isInside(int x, int y) const;

    std::vector<Shape*>& getShapes();

    void selectShape(int x, int y);
    Shape* getSelectedShape();

    //void deleteSelectedShape();         <- ainda vou implementar

private:
    struct Fill
    {
        Point point;
        Color color;
    };

    int width;
    int height;

    int x;
    int y;

    std::vector<Shape*> shapes;
    std::vector<Fill> fills;

    Shape* selectedShape;
    void drawSelectionIndicator();
};

#endif // CANVAS_H
