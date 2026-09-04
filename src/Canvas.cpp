#include "Canvas.h"
#include "Context.h"
#include "Line.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "Curve.h"
#include "FloodFill.h"
#include "Transform.h"

#include <cmath>
#include <algorithm>

// Cria o Canvas
Canvas::Canvas(int width, int height)
{
    this->width = width;
    this->height = height;
    x = 0;
    y = 40;
    selectedShape = nullptr;
}

// Libera as figuras
Canvas::~Canvas()
{
    for (Shape* shape : shapes)
        delete shape;
}

// Adiciona uma figura
void Canvas::addShape(Shape* shape)
{
    if (shape != nullptr)
        shapes.push_back(shape);
}

// Desenha o Canvas
void Canvas::draw()
{
    for (Shape* shape : shapes)
        if (shape != nullptr)
            shape->draw();

    FloodFill floodFill;

    for (const Fill& fill : fills)
        floodFill.fill(fill.point, fill.color);

    if (selectedShape != nullptr)
    {
        drawSelectionIndicator();
        drawSelectionHandles();
    }
}

// Limpa o Canvas
void Canvas::clear()
{
    Context::getInstance()->setViewport(x, y, width, height);

    Line line;

    for (int px = x; px < x + width; px++)
        for (int py = y; py < y + height; py++)
            line.setPixel(px, py, 255, 255, 255);
}

// Verifica os limites
bool Canvas::isInside(int px, int py) const
{
    return px >= x && px < x + width && py >= y && py < y + height;
}

// Retorna as figuras
std::vector<Shape*>& Canvas::getShapes()
{
    return shapes;
}

// Retorna a seleção
Shape* Canvas::getSelectedShape()
{
    return selectedShape;
}

// Guarda um Flood Fill
void Canvas::addFloodFill(Point startPoint, Color newColor)
{
    if (isInside(startPoint.getX(), startPoint.getY()))
        fills.push_back({startPoint, newColor});
}

// Calcula os limites
bool Canvas::getShapeBounds(Shape* shape, int& minX, int& minY, int& maxX, int& maxY)
{
    if (shape == nullptr)
        return false;

    minX = minY = 999999;
    maxX = maxY = -999999;

    if (Line* line = dynamic_cast<Line*>(shape))
    {
        Point p1 = line->getStart();
        Point p2 = line->getEnd();

        minX = std::min(p1.getX(), p2.getX());
        minY = std::min(p1.getY(), p2.getY());
        maxX = std::max(p1.getX(), p2.getX());
        maxY = std::max(p1.getY(), p2.getY());
        return true;
    }

    if (Circle* circle = dynamic_cast<Circle*>(shape))
    {
        Point c = circle->getXy();
        double r = circle->getRadius();

        minX = c.getX() - r;
        minY = c.getY() - r;
        maxX = c.getX() + r;
        maxY = c.getY() + r;
        return true;
    }

    if (Rectangle* rect = dynamic_cast<Rectangle*>(shape))
    {
        Point p = rect->getXy();
        double cx = p.getX() + rect->getWidth() / 2.0;
        double cy = p.getY() + rect->getHeight() / 2.0;
        double a = rect->getRotation() * M_PI / 180.0;
        double cosA = cos(a), sinA = sin(a);

        Point corners[4] = {
            Point(p.getX(), p.getY()),
            Point(p.getX() + rect->getWidth(), p.getY()),
            Point(p.getX() + rect->getWidth(), p.getY() + rect->getHeight()),
            Point(p.getX(), p.getY() + rect->getHeight())
        };

        for (Point corner : corners)
        {
            double dx = corner.getX() - cx;
            double dy = corner.getY() - cy;
            int rx = (int)(dx * cosA - dy * sinA + cx);
            int ry = (int)(dx * sinA + dy * cosA + cy);

            minX = std::min(minX, rx);
            minY = std::min(minY, ry);
            maxX = std::max(maxX, rx);
            maxY = std::max(maxY, ry);
        }

        return true;
    }

    if (Polygon* polygon = dynamic_cast<Polygon*>(shape))
    {
        std::list<Point> points = polygon->getPontos();

        if (points.empty())
            return false;

        for (Point p : points)
        {
            minX = std::min(minX, p.getX());
            minY = std::min(minY, p.getY());
            maxX = std::max(maxX, p.getX());
            maxY = std::max(maxY, p.getY());
        }

        return true;
    }

    if (Curve* curve = dynamic_cast<Curve*>(shape))
    {
        for (int i = 0; i < 4; i++)
        {
            Point p = curve->getPoint(i);

            minX = std::min(minX, p.getX());
            minY = std::min(minY, p.getY());
            maxX = std::max(maxX, p.getX());
            maxY = std::max(maxY, p.getY());
        }

        return true;
    }

    return false;
}

// Calcula distância ao segmento
double Canvas::distanceToSegment(int px, int py, Point p1, Point p2)
{
    double dx = p2.getX() - p1.getX();
    double dy = p2.getY() - p1.getY();
    double length = dx * dx + dy * dy;

    double t = length == 0 ? 0 :
        ((px - p1.getX()) * dx + (py - p1.getY()) * dy) / length;

    t = std::max(0.0, std::min(1.0, t));

    double cx = p1.getX() + t * dx;
    double cy = p1.getY() + t * dy;
    double ex = px - cx;
    double ey = py - cy;

    return sqrt(ex * ex + ey * ey);
}

// Verifica proximidade
bool Canvas::isPointNearShape(Shape* shape, int px, int py, double tolerance)
{
    if (shape == nullptr)
        return false;

    if (Line* line = dynamic_cast<Line*>(shape))
        return distanceToSegment(px, py, line->getStart(), line->getEnd()) <= tolerance;

    if (Circle* circle = dynamic_cast<Circle*>(shape))
    {
        Point c = circle->getXy();
        double dx = px - c.getX();
        double dy = py - c.getY();

        return fabs(sqrt(dx * dx + dy * dy) - circle->getRadius()) <= tolerance;
    }

    if (Polygon* polygon = dynamic_cast<Polygon*>(shape))
    {
        std::list<Point> points = polygon->getPontos();

        if (points.size() < 2)
            return false;

        Point prev = points.back();

        for (Point cur : points)
        {
            if (distanceToSegment(px, py, prev, cur) <= tolerance)
                return true;

            prev = cur;
        }

        return false;
    }

    if (Curve* curve = dynamic_cast<Curve*>(shape))
    {
        Point prev = curve->getPoint(0);

        for (double t = 0.001; t <= 1.0; t += 0.001)
        {
            double mt = 1 - t;

            double x =
                mt * mt * mt * curve->getPoint(0).getX() +
                3 * mt * mt * t * curve->getPoint(1).getX() +
                3 * mt * t * t * curve->getPoint(2).getX() +
                t * t * t * curve->getPoint(3).getX();

            double y =
                mt * mt * mt * curve->getPoint(0).getY() +
                3 * mt * mt * t * curve->getPoint(1).getY() +
                3 * mt * t * t * curve->getPoint(2).getY() +
                t * t * t * curve->getPoint(3).getY();

            Point cur((int)x, (int)y);

            if (distanceToSegment(px, py, prev, cur) <= tolerance)
                return true;

            prev = cur;
        }

        return false;
    }

    if (Rectangle* rect = dynamic_cast<Rectangle*>(shape))
    {
        Point p = rect->getXy();

        double cx = p.getX() + rect->getWidth() / 2.0;
        double cy = p.getY() + rect->getHeight() / 2.0;
        double a = -rect->getRotation() * M_PI / 180.0;

        double dx = px - cx;
        double dy = py - cy;

        double lx = dx * cos(a) - dy * sin(a) + cx;
        double ly = dx * sin(a) + dy * cos(a) + cy;

        int left = p.getX();
        int right = left + rect->getWidth();
        int top = p.getY();
        int bottom = top + rect->getHeight();

        return distanceToSegment(lx, ly, Point(left, top), Point(right, top)) <= tolerance ||
               distanceToSegment(lx, ly, Point(right, top), Point(right, bottom)) <= tolerance ||
               distanceToSegment(lx, ly, Point(right, bottom), Point(left, bottom)) <= tolerance ||
               distanceToSegment(lx, ly, Point(left, bottom), Point(left, top)) <= tolerance;
    }

    return false;
}

// Seleciona a figura
void Canvas::selectShape(int px, int py)
{
    selectedShape = nullptr;

    if (!isInside(px, py))
        return;

    for (int i = shapes.size() - 1; i >= 0; i--)
        if (isPointNearShape(shapes[i], px, py, 5))
        {
            selectedShape = shapes[i];
            return;
        }
}

// Desenha caixa
void Canvas::drawBoundingBox(int minX, int minY, int maxX, int maxY, Color color)
{
    Rectangle(Point(minX, minY), maxX - minX, maxY - minY, color).draw();
}

// Desenha seleção
void Canvas::drawSelectionIndicator()
{
    int minX, minY, maxX, maxY;

    if (selectedShape != nullptr &&
        getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        drawBoundingBox(minX - 5, minY - 5, maxX + 5, maxY + 5, Color(0, 0, 255));
}

// Desenha um handle
void Canvas::drawHandle(int x, int y, Color color)
{
    Line line;

    for (int px = x - 5; px <= x + 5; px++)
        for (int py = y - 5; py <= y + 5; py++)
            line.setPixel(px, py, color);
}

// Desenha handles
void Canvas::drawSelectionHandles()
{
    int minX, minY, maxX, maxY;

    if (selectedShape == nullptr ||
        !getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    Color blue(0, 0, 255);
    int cx = (minX + maxX) / 2;

    drawHandle(minX, minY, blue);
    drawHandle(maxX, minY, blue);
    drawHandle(minX, maxY, blue);
    drawHandle(maxX, maxY, blue);
    drawHandle(cx, minY - 25, blue);
    drawHandle(cx, maxY + 25, blue);
}

// Detecta o handle
Canvas::HandleType Canvas::getHandleAt(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return HANDLE_NONE;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return HANDLE_NONE;

    int t = 8;
    int cx = (minX + maxX) / 2;

    if (abs(mouseX - minX) <= t && abs(mouseY - minY) <= t) return HANDLE_SCALE_TOP_LEFT;
    if (abs(mouseX - maxX) <= t && abs(mouseY - minY) <= t) return HANDLE_SCALE_TOP_RIGHT;
    if (abs(mouseX - minX) <= t && abs(mouseY - maxY) <= t) return HANDLE_SCALE_BOTTOM_LEFT;
    if (abs(mouseX - maxX) <= t && abs(mouseY - maxY) <= t) return HANDLE_SCALE_BOTTOM_RIGHT;
    if (abs(mouseX - cx) <= t && abs(mouseY - minY + 25) <= t) return HANDLE_ROTATE;
    if (abs(mouseX - cx) <= t && abs(mouseY - maxY - 25) <= t) return HANDLE_TRANSLATE;

    return HANDLE_NONE;
}

// Inicia a escala
void Canvas::startScale(HandleType handle, int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    originalScalePoints.clear();
    originalCurvePoints.clear();

    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
        for (Point p : polygon->getPontos())
            originalScalePoints.push_back(p);

    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
        for (int i = 0; i < 4; i++)
            originalCurvePoints.push_back(curve->getPoint(i));

    originalWidth = maxX - minX;
    originalHeight = maxY - minY;
    scaleStartMouse = Point(mouseX, mouseY);

    if (handle == HANDLE_SCALE_TOP_LEFT) scaleReference = Point(maxX, maxY);
    if (handle == HANDLE_SCALE_TOP_RIGHT) scaleReference = Point(minX, maxY);
    if (handle == HANDLE_SCALE_BOTTOM_LEFT) scaleReference = Point(maxX, minY);
    if (handle == HANDLE_SCALE_BOTTOM_RIGHT) scaleReference = Point(minX, minY);
}

// Atualiza a escala
void Canvas::updateScale(HandleType handle, int mouseX, int mouseY)
{
    if (selectedShape == nullptr || originalWidth <= 0 || originalHeight <= 0)
        return;

    double w = 0, h = 0;

    if (handle == HANDLE_SCALE_TOP_LEFT) { w = scaleReference.getX() - mouseX; h = scaleReference.getY() - mouseY; }
    if (handle == HANDLE_SCALE_TOP_RIGHT) { w = mouseX - scaleReference.getX(); h = scaleReference.getY() - mouseY; }
    if (handle == HANDLE_SCALE_BOTTOM_LEFT) { w = scaleReference.getX() - mouseX; h = mouseY - scaleReference.getY(); }
    if (handle == HANDLE_SCALE_BOTTOM_RIGHT) { w = mouseX - scaleReference.getX(); h = mouseY - scaleReference.getY(); }

    if (w <= 0 || h <= 0)
        return;

    w = std::max(5.0, w);
    h = std::max(5.0, h);

    double sx = w / originalWidth;
    double sy = h / originalHeight;

    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
    {
        std::list<Point> points(originalScalePoints.begin(), originalScalePoints.end());
        polygon->setPontos(points);
        polygon->scaleFromMouse(sx, sy, scaleReference);
        return;
    }

    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
    {
        if (originalCurvePoints.size() != 4)
            return;

        for (int i = 0; i < 4; i++)
            curve->setPoint(i, originalCurvePoints[i]);

        curve->scaleFromMouse(sx, sy, scaleReference);
        return;
    }

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    double cw = maxX - minX;
    double ch = maxY - minY;

    if (cw <= 0 || ch <= 0)
        return;

    selectedShape->scaleFromMouse(w / cw, h / ch, scaleReference);
}

// Deleta a seleção
void Canvas::deleteSelectedShape()
{
    auto it = std::find(shapes.begin(), shapes.end(), selectedShape);

    if (it == shapes.end())
        return;

    delete *it;
    shapes.erase(it);
    selectedShape = nullptr;
}

// Limpa seleção
void Canvas::clearSelection()
{
    selectedShape = nullptr;
}

// Inicia rotação
void Canvas::startRotate(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    rotationCenter = Point((minX + maxX) / 2, (minY + maxY) / 2);
    rotationStartMouseX = mouseX;
    rotationTotalAngle = 0;
    originalRotationPoints.clear();

    if (Line* line = dynamic_cast<Line*>(selectedShape))
    {
        originalRotationStart = line->getStart();
        originalRotationEnd = line->getEnd();
        return;
    }

    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
    {
        for (Point p : polygon->getPontos())
            originalRotationPoints.push_back(p);
        return;
    }

    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
        for (int i = 0; i < 4; i++)
            originalRotationPoints.push_back(curve->getPoint(i));
}

// Atualiza rotação
void Canvas::updateRotate(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    rotationTotalAngle = (mouseX - rotationStartMouseX) * 0.4;

    Transform transform;

    if (Line* line = dynamic_cast<Line*>(selectedShape))
    {
        Point points[2] = {originalRotationStart, originalRotationEnd};

        transform.rotate(points, 2, rotationTotalAngle, rotationCenter);

        line->setStart(points[0]);
        line->setEnd(points[1]);
        return;
    }

    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
    {
        if (originalRotationPoints.empty())
            return;

        std::vector<Point> points = originalRotationPoints;

        transform.rotate(points.data(), points.size(), rotationTotalAngle, rotationCenter);

        std::list<Point> result(points.begin(), points.end());
        polygon->setPontos(result);
        return;
    }

    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
    {
        if (originalRotationPoints.size() != 4)
            return;

        Point points[4];

        for (int i = 0; i < 4; i++)
            points[i] = originalRotationPoints[i];

        transform.rotate(points, 4, rotationTotalAngle, rotationCenter);

        for (int i = 0; i < 4; i++)
            curve->setPoint(i, points[i]);

        return;
    }

    selectedShape->rotate(rotationTotalAngle, rotationCenter);
}
