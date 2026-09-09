#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <list>

#include "Shape.h"
#include "Point.h"
#include "Color.h"

class Canvas
{
public:
    enum HandleType
    {
        HANDLE_NONE,
        HANDLE_SCALE_TOP_LEFT,
        HANDLE_SCALE_TOP_RIGHT,
        HANDLE_SCALE_BOTTOM_LEFT,
        HANDLE_SCALE_BOTTOM_RIGHT,
        HANDLE_ROTATE,
        HANDLE_TRANSLATE
    };

    Canvas(int width, int height);
    ~Canvas();

    void addShape(Shape* shape);
    void draw();
    void clear();
    bool isInside(int px, int py) const;
    std::vector<Shape*>& getShapes();
    Shape* getSelectedShape();
    void addFloodFill(Point startPoint, Color newColor);
    void selectShape(int px, int py);
    void drawSelectionIndicator();
    void drawSelectionHandles();
    HandleType getHandleAt(int mouseX, int mouseY);
    void startScale(HandleType handle, int mouseX, int mouseY);
    void updateScale(HandleType handle, int mouseX, int mouseY);
    void deleteSelectedShape();
    void clearSelection();

    void startRotate(int mouseX, int mouseY);
    void updateRotate(int mouseX, int mouseY);


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

    Point scaleStartMouse;
    Point scaleReference;

    double originalWidth;
    double originalHeight;

    std::vector<Point> originalScalePoints;
    std::vector<Point> originalCurvePoints;

    Point rotationCenter;
    int rotationStartMouseX;
    double rotationTotalAngle;

    std::vector<Point> originalRotationPoints;
    Point originalRotationStart;
    Point originalRotationEnd;

    bool getShapeBounds(Shape* shape, int& minX, int& minY, int& maxX, int& maxY);
    double distanceToSegment(int px, int py, Point p1, Point p2);
    bool isPointNearShape(Shape* shape, int px, int py, double tolerance);

    void drawHandle(int x, int y, Color color);
    void drawBoundingBox(int minX, int minY, int maxX, int maxY, Color color);

    Point originalRectXy;
    double originalRectWidth;
    double originalRectHeight;
    double originalRectRotation;

};

#endif
