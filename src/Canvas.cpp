#include "Canvas.h"
#include "Context.h"
#include "Line.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "Curve.h"
#include "FloodFill.h"

#include <cmath>
#include <algorithm>

// Cria o Canvas e define sua posição
Canvas::Canvas(int width, int height)
{
    this->width = width;
    this->height = height;
    this->x = 0;
    this->y = 40;
    this->selectedShape = nullptr;
}

// Libera as figuras do Canvas
Canvas::~Canvas()
{
    for (Shape* shape : shapes)
        delete shape;
}

// Adiciona uma figura no Canvas
void Canvas::addShape(Shape* shape)
{
    if (shape != nullptr)
        shapes.push_back(shape);
}

// Desenha as figuras e os controles da seleção
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

// Limpa a área do Canvas
void Canvas::clear()
{
    Context* context = Context::getInstance();
    context->setViewport(x, y, width, height);

    Line line;

    for (int px = x; px < x + width; px++)
        for (int py = y; py < y + height; py++)
            line.setPixel(px, py, 255, 255, 255);
}

// Verifica se o ponto está dentro do Canvas
bool Canvas::isInside(int px, int py) const
{
    return px >= x && px < x + width && py >= y && py < y + height;
}

// Retorna as figuras do Canvas
std::vector<Shape*>& Canvas::getShapes()
{
    return shapes;
}

// Retorna a figura selecionada
Shape* Canvas::getSelectedShape()
{
    return selectedShape;
}

// Guarda um Flood Fill para ser aplicado novamente
void Canvas::addFloodFill(Point startPoint, Color newColor)
{
    if (!isInside(startPoint.getX(), startPoint.getY()))
        return;

    Fill fill;
    fill.point = startPoint;
    fill.color = newColor;
    fills.push_back(fill);
}

// Calcula os limites da figura
bool Canvas::getShapeBounds(Shape* shape, int& minX, int& minY, int& maxX, int& maxY)
{
    if (shape == nullptr)
        return false;

    minX = 999999;
    minY = 999999;
    maxX = -999999;
    maxY = -999999;

    Line* line = dynamic_cast<Line*>(shape);

    if (line != nullptr)
    {
        Point p1 = line->getStart();
        Point p2 = line->getEnd();

        minX = std::min(p1.getX(), p2.getX());
        minY = std::min(p1.getY(), p2.getY());
        maxX = std::max(p1.getX(), p2.getX());
        maxY = std::max(p1.getY(), p2.getY());

        return true;
    }

    Circle* circle = dynamic_cast<Circle*>(shape);

    if (circle != nullptr)
    {
        Point center = circle->getXy();
        double radius = circle->getRadius();

        minX = center.getX() - radius;
        minY = center.getY() - radius;
        maxX = center.getX() + radius;
        maxY = center.getY() + radius;

        return true;
    }

    Rectangle* rectangle = dynamic_cast<Rectangle*>(shape);

    if (rectangle != nullptr)
    {
        Point p = rectangle->getXy();

        minX = p.getX();
        minY = p.getY();
        maxX = p.getX() + rectangle->getWidth();
        maxY = p.getY() + rectangle->getHeight();

        return true;
    }

    Polygon* polygon = dynamic_cast<Polygon*>(shape);

    if (polygon != nullptr)
    {
        std::list<Point> points = polygon->getPontos();

        if (points.empty())
            return false;

        for (Point point : points)
        {
            minX = std::min(minX, point.getX());
            minY = std::min(minY, point.getY());
            maxX = std::max(maxX, point.getX());
            maxY = std::max(maxY, point.getY());
        }

        return true;
    }

    Curve* curve = dynamic_cast<Curve*>(shape);

    if (curve != nullptr)
    {
        for (int i = 0; i < 4; i++)
        {
            Point point = curve->getPoint(i);

            minX = std::min(minX, point.getX());
            minY = std::min(minY, point.getY());
            maxX = std::max(maxX, point.getX());
            maxY = std::max(maxY, point.getY());
        }

        return true;
    }

    return false;
}

// Calcula a distância entre um ponto e uma linha
double Canvas::distanceToSegment(int px, int py, Point p1, Point p2)
{
    double dx = p2.getX() - p1.getX();
    double dy = p2.getY() - p1.getY();
    double length2 = dx * dx + dy * dy;

    double t = 0;

    if (length2 != 0)
        t = ((px - p1.getX()) * dx + (py - p1.getY()) * dy) / length2;

    t = std::max(0.0, std::min(1.0, t));

    double closestX = p1.getX() + t * dx;
    double closestY = p1.getY() + t * dy;

    double distanceX = px - closestX;
    double distanceY = py - closestY;

    return sqrt(distanceX * distanceX + distanceY * distanceY);
}

// Verifica se o ponto está próximo da figura
bool Canvas::isPointNearShape(Shape* shape, int px, int py, double tolerance)
{
    if (shape == nullptr)
        return false;

    Line* line = dynamic_cast<Line*>(shape);

    if (line != nullptr)
        return distanceToSegment(px, py, line->getStart(), line->getEnd()) <= tolerance;

    Circle* circle = dynamic_cast<Circle*>(shape);

    if (circle != nullptr)
    {
        Point center = circle->getXy();
        double dx = px - center.getX();
        double dy = py - center.getY();
        double distance = sqrt(dx * dx + dy * dy);

        return fabs(distance - circle->getRadius()) <= tolerance;
    }

    Polygon* polygon = dynamic_cast<Polygon*>(shape);

    if (polygon != nullptr)
    {
        std::list<Point> points = polygon->getPontos();

        if (points.size() < 2)
            return false;

        Point previous = points.back();

        for (Point current : points)
        {
            if (distanceToSegment(px, py, previous, current) <= tolerance)
                return true;

            previous = current;
        }

        return false;
    }

    Curve* curve = dynamic_cast<Curve*>(shape);

    if (curve != nullptr)
    {
        Point previous = curve->getPoint(0);

        for (double t = 0.001; t <= 1.0; t += 0.001)
        {
            double x = pow(1 - t, 3) * curve->getPoint(0).getX() + 3 * pow(1 - t, 2) * t * curve->getPoint(1).getX() + 3 * (1 - t) * pow(t, 2) * curve->getPoint(2).getX() + pow(t, 3) * curve->getPoint(3).getX();

            double y = pow(1 - t, 3) * curve->getPoint(0).getY() + 3 * pow(1 - t, 2) * t * curve->getPoint(1).getY() + 3 * (1 - t) * pow(t, 2) * curve->getPoint(2).getY() + pow(t, 3) * curve->getPoint(3).getY();

            Point current((int)x, (int)y);

            if (distanceToSegment(px, py, previous, current) <= tolerance)
                return true;

            previous = current;
        }
    }

    Rectangle* rectangle = dynamic_cast<Rectangle*>(shape);

    if (rectangle != nullptr)
    {
        Point p = rectangle->getXy();

        int left = p.getX();
        int right = left + rectangle->getWidth();
        int top = p.getY();
        int bottom = top + rectangle->getHeight();

        return distanceToSegment(px, py, Point(left, top), Point(right, top)) <= tolerance ||
               distanceToSegment(px, py, Point(right, top), Point(right, bottom)) <= tolerance ||
               distanceToSegment(px, py, Point(right, bottom), Point(left, bottom)) <= tolerance ||
               distanceToSegment(px, py, Point(left, bottom), Point(left, top)) <= tolerance;
    }

    return false;
}

// Seleciona a figura clicada
void Canvas::selectShape(int px, int py)
{
    if (!isInside(px, py))
    {
        selectedShape = nullptr;
        return;
    }

    for (int i = (int)shapes.size() - 1; i >= 0; i--)
    {
        if (isPointNearShape(shapes[i], px, py, 5.0))
        {
            selectedShape = shapes[i];
            return;
        }
    }

    selectedShape = nullptr;
}

// Desenha a caixa de seleção usando o Rectangle
void Canvas::drawBoundingBox(int minX, int minY, int maxX, int maxY, Color color)
{
    Rectangle rectangle(Point(minX, minY), maxX - minX, maxY - minY, color);
    rectangle.draw();
}

// Desenha a caixa ao redor da figura selecionada
void Canvas::drawSelectionIndicator()
{
    if (selectedShape == nullptr)
        return;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    drawBoundingBox(minX - 5, minY - 5, maxX + 5, maxY + 5, Color(0, 0, 255));
}

// Desenha um controle da seleção
void Canvas::drawHandle(int x, int y, Color color)
{
    Line line;
    int size = 5;

    for (int px = x - size; px <= x + size; px++)
        for (int py = y - size; py <= y + size; py++)
            line.setPixel(px, py, color);
}

// Desenha os controles de escala, rotação e movimentação
void Canvas::drawSelectionHandles()
{
    if (selectedShape == nullptr)
        return;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    Color blue(0, 0, 255);

    drawHandle(minX, minY, blue);
    drawHandle(maxX, minY, blue);
    drawHandle(minX, maxY, blue);
    drawHandle(maxX, maxY, blue);

    int centerX = (minX + maxX) / 2;

    drawHandle(centerX, minY - 25, blue);
    drawHandle(centerX, maxY + 25, blue);
}

// Verifica qual controle foi clicado
Canvas::HandleType Canvas::getHandleAt(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return HANDLE_NONE;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return HANDLE_NONE;

    int tolerance = 8;
    int centerX = (minX + maxX) / 2;

    if (std::abs(mouseX - minX) <= tolerance && std::abs(mouseY - minY) <= tolerance)
        return HANDLE_SCALE_TOP_LEFT;

    if (std::abs(mouseX - maxX) <= tolerance && std::abs(mouseY - minY) <= tolerance)
        return HANDLE_SCALE_TOP_RIGHT;

    if (std::abs(mouseX - minX) <= tolerance && std::abs(mouseY - maxY) <= tolerance)
        return HANDLE_SCALE_BOTTOM_LEFT;

    if (std::abs(mouseX - maxX) <= tolerance && std::abs(mouseY - maxY) <= tolerance)
        return HANDLE_SCALE_BOTTOM_RIGHT;

    if (std::abs(mouseX - centerX) <= tolerance && std::abs(mouseY - (minY - 25)) <= tolerance)
        return HANDLE_ROTATE;

    if (std::abs(mouseX - centerX) <= tolerance && std::abs(mouseY - (maxY + 25)) <= tolerance)
        return HANDLE_TRANSLATE;

    return HANDLE_NONE;
}

// Salva os pontos originais antes da escala
void Canvas::startScale(HandleType handle, int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    originalScalePoints.clear();
    originalCurvePoints.clear();

    Polygon* polygon = dynamic_cast<Polygon*>(selectedShape);

    if (polygon != nullptr)
    {
        std::list<Point> points = polygon->getPontos();

        for (Point point : points)
            originalScalePoints.push_back(point);
    }

    Curve* curve = dynamic_cast<Curve*>(selectedShape);

    if (curve != nullptr)
    {
        for (int i = 0; i < 4; i++)
            originalCurvePoints.push_back(curve->getPoint(i));
    }

    originalWidth = maxX - minX;
    originalHeight = maxY - minY;

    scaleStartMouse = Point(mouseX, mouseY);

    if (handle == HANDLE_SCALE_TOP_LEFT)
        scaleReference = Point(maxX, maxY);
    else if (handle == HANDLE_SCALE_TOP_RIGHT)
        scaleReference = Point(minX, maxY);
    else if (handle == HANDLE_SCALE_BOTTOM_LEFT)
        scaleReference = Point(maxX, minY);
    else if (handle == HANDLE_SCALE_BOTTOM_RIGHT)
        scaleReference = Point(minX, minY);
}

// Atualiza o tamanho da figura durante a escala
void Canvas::updateScale(HandleType handle, int mouseX, int mouseY)
{
    if (selectedShape == nullptr || originalWidth <= 0 || originalHeight <= 0)
        return;

    double newWidth;
    double newHeight;

    if (handle == HANDLE_SCALE_TOP_LEFT)
    {
        newWidth = scaleReference.getX() - mouseX;
        newHeight = scaleReference.getY() - mouseY;
    }
    else if (handle == HANDLE_SCALE_TOP_RIGHT)
    {
        newWidth = mouseX - scaleReference.getX();
        newHeight = scaleReference.getY() - mouseY;
    }
    else if (handle == HANDLE_SCALE_BOTTOM_LEFT)
    {
        newWidth = scaleReference.getX() - mouseX;
        newHeight = mouseY - scaleReference.getY();
    }
    else if (handle == HANDLE_SCALE_BOTTOM_RIGHT)
    {
        newWidth = mouseX - scaleReference.getX();
        newHeight = mouseY - scaleReference.getY();
    }
    else
    {
        return;
    }

    newWidth = std::max(5.0, newWidth);
    newHeight = std::max(5.0, newHeight);

    double sx = newWidth / originalWidth;
    double sy = newHeight / originalHeight;

    Polygon* polygon = dynamic_cast<Polygon*>(selectedShape);

    if (polygon != nullptr)
    {
        if (originalScalePoints.empty())
            return;

        std::list<Point> points;

        for (Point point : originalScalePoints)
            points.push_back(point);

        polygon->setPontos(points);
        polygon->scaleFromMouse(sx, sy, scaleReference);

        return;
    }

    Curve* curve = dynamic_cast<Curve*>(selectedShape);

    if (curve != nullptr)
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

    double currentWidth = maxX - minX;
    double currentHeight = maxY - minY;

    if (currentWidth <= 0 || currentHeight <= 0)
        return;

    double currentSx = newWidth / currentWidth;
    double currentSy = newHeight / currentHeight;

    if (currentSx <= 0 || currentSy <= 0)
        return;

    selectedShape->scaleFromMouse(currentSx, currentSy, scaleReference);
}
void Canvas::deleteSelectedShape()
{
    if (selectedShape == nullptr)
        return;

    auto it = std::find(shapes.begin(), shapes.end(), selectedShape);

    if (it != shapes.end())
    {
        delete *it;
        shapes.erase(it);
    }

    selectedShape = nullptr;
}

void Canvas::clearSelection()
{
    selectedShape = nullptr;
}
