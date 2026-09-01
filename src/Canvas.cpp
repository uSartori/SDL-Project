#include "Canvas.h"
#include "Context.h"
#include "Line.h"
#include "FloodFill.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "Curve.h"

#include <cmath>
#include <algorithm>

Canvas::Canvas(int width, int height)
{
    this->width = width;
    this->height = height;

    // O Canvas começa abaixo da Toolbar
    this->x = 0;
    this->y = 40;

    // Nenhuma figura começa selecionada
    this->selectedShape = nullptr;
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

    // Desenha o indicador visual da figura selecionada
    drawSelectionIndicator();
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
    // Verifica se o clique esta dentro do Canvas
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

// Seleciona uma figura pelo ponto clicado
void Canvas::selectShape(int px, int py)
{
    // Se clicar fora do Canvas, remove a seleção
    if (!isInside(px, py))
    {
        selectedShape = nullptr;
        return;
    }

    // Pixels entre o ponto do clique do mouse e um dos pixels que compoe o objeto
    const double maxDistance = 5.0;

    // A ultima figura desenhada sera selecionada primeiro
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        Shape* shape = shapes[i];

        if (shape == nullptr)
        {
            continue;
        }

        // Verifica se e uma linha
        Line* line = dynamic_cast<Line*>(shape);

        if (line != nullptr)
        {
            Point p1 = line->getStart();
            Point p2 = line->getEnd();

            double x1 = p1.getX();
            double y1 = p1.getY();

            double x2 = p2.getX();
            double y2 = p2.getY();

            double dx = x2 - x1;
            double dy = y2 - y1;

            double comprimento2 =
                dx * dx + dy * dy;

            double t;

            if (comprimento2 == 0)
            {
                t = 0;
            }
            else
            {
                t = ((px - x1) * dx + (py - y1) * dy) / comprimento2;
            }

            // Mantem o ponto mais proximo dentro do segmento
            if (t < 0)
            {
                t = 0;
            }

            if (t > 1)
            {
                t = 1;
            }

            double pontoMaisProximoX =
                x1 + t * dx;

            double pontoMaisProximoY =
                y1 + t * dy;

            double distanciaX =
                px - pontoMaisProximoX;

            double distanciaY =
                py - pontoMaisProximoY;

            double distancia =
                sqrt(
                    distanciaX * distanciaX +
                    distanciaY * distanciaY
                );

            if (distancia <= maxDistance)
            {
                selectedShape = shape;
                return;
            }
        }

        // Verifica se e um circulo
        Circle* circle =
            dynamic_cast<Circle*>(shape);

        if (circle != nullptr)
        {
            Point center = circle->getXy();

            double dx =
                px - center.getX();

            double dy =
                py - center.getY();

            double distanciaCentro =
                sqrt(dx * dx + dy * dy);

            double distanciaBorda =
                fabs(
                    distanciaCentro -
                    circle->getRadius()
                );

            if (distanciaBorda <= maxDistance)
            {
                selectedShape = shape;
                return;
            }
        }

        // Verifica se e um retangulo
        Rectangle* rectangle =
            dynamic_cast<Rectangle*>(shape);

        if (rectangle != nullptr)
        {
            Point p =
                rectangle->getXy();

            double left = p.getX();
            double right =
                p.getX() +
                rectangle->getWidth();

            double top = p.getY();
            double bottom =
                p.getY() +
                rectangle->getHeight();

            // Distancia para cada lado
            double dLeft =
                fabs(px - left);

            double dRight =
                fabs(px - right);

            double dTop =
                fabs(py - top);

            double dBottom =
                fabs(py - bottom);

            bool dentroX =
                px >= left &&
                px <= right;

            bool dentroY =
                py >= top &&
                py <= bottom;

            if ((dLeft <= maxDistance && dentroY) ||
                    (dRight <= maxDistance && dentroY) ||
                    (dTop <= maxDistance && dentroX) ||
                    (dBottom <= maxDistance && dentroX))
            {
                selectedShape = shape;
                return;
            }
        }

        // Verifica se e um poligono
        Polygon* polygon =
            dynamic_cast<Polygon*>(shape);

        if (polygon != nullptr)
        {
            list<Point> pontos =
                polygon->getPontos();

            if (pontos.size() >= 2)
            {
                Point anterior =
                    pontos.back();

                for (Point atual : pontos)
                {
                    double x1 = anterior.getX();
                    double y1 = anterior.getY();

                    double x2 = atual.getX();
                    double y2 = atual.getY();

                    double dx = x2 - x1;
                    double dy = y2 - y1;

                    double comprimento2 =
                        dx * dx + dy * dy;

                    double t;

                    if (comprimento2 == 0)
                    {
                        t = 0;
                    }
                    else
                    {
                        t = ((px - x1) * dx + (py - y1) * dy) / comprimento2;
                    }

                    if (t < 0)
                    {
                        t = 0;
                    }

                    if (t > 1)
                    {
                        t = 1;
                    }

                    double pontoX =
                        x1 + t * dx;

                    double pontoY =
                        y1 + t * dy;

                    double distanciaX =
                        px - pontoX;

                    double distanciaY =
                        py - pontoY;

                    double distancia =
                        sqrt(
                            distanciaX * distanciaX +
                            distanciaY * distanciaY
                        );

                    if (distancia <= maxDistance)
                    {
                        selectedShape = shape;
                        return;
                    }

                    anterior = atual;
                }
            }
        }

        // Verifica se e uma curva
        Curve* curve =
            dynamic_cast<Curve*>(shape);

        if (curve != nullptr)
        {
            Point previous =
                curve->getPoint(0);

            for (double t = 0.001;
                    t <= 1.0;
                    t += 0.001)
            {
                double x =
                    pow(1 - t, 3) *
                    curve->getPoint(0).getX()
                    +
                    3 * pow(1 - t, 2) * t *
                    curve->getPoint(1).getX()
                    +
                    3 * (1 - t) * pow(t, 2) *
                    curve->getPoint(2).getX()
                    +
                    pow(t, 3) *
                    curve->getPoint(3).getX();

                double y =
                    pow(1 - t, 3) *
                    curve->getPoint(0).getY()
                    +
                    3 * pow(1 - t, 2) * t *
                    curve->getPoint(1).getY()
                    +
                    3 * (1 - t) * pow(t, 2) *
                    curve->getPoint(2).getY()
                    +
                    pow(t, 3) *
                    curve->getPoint(3).getY();

                double distanciaX =
                    px - x;

                double distanciaY =
                    py - y;

                double distancia =
                    sqrt(
                        distanciaX * distanciaX +
                        distanciaY * distanciaY
                    );

                if (distancia <= maxDistance)
                {
                    selectedShape = shape;
                    return;
                }

                previous =
                    Point((int)x, (int)y);
            }
        }
    }

    // Nenhuma figura encontrada
    selectedShape = nullptr;
}

// Desenha o indicador visual da seleção
void Canvas::drawSelectionIndicator()
{
    // Se não existe figura selecionada, não faz nada
    if (selectedShape == nullptr)
    {
        return;
    }

    Line line;

    // Cor utilizada para indicar a selecao
    Color selectionColor(0, 0, 255);

    Line* selectedLine =
        dynamic_cast<Line*>(selectedShape);

    if (selectedLine != nullptr)
    {
        Point p1 = selectedLine->getStart();
        Point p2 = selectedLine->getEnd();

        int minX = std::min(p1.getX(), p2.getX());
        int maxX = std::max(p1.getX(), p2.getX());

        int minY = std::min(p1.getY(), p2.getY());
        int maxY = std::max(p1.getY(), p2.getY());

        // Espaco para destacar a selecao
        minX -= 5;
        maxX += 5;
        minY -= 5;
        maxY += 5;

        line.drawWuLine(
            minX, minY,
            maxX, minY,
            selectionColor
        );

        line.drawWuLine(
            maxX, minY,
            maxX, maxY,
            selectionColor
        );

        line.drawWuLine(
            maxX, maxY,
            minX, maxY,
            selectionColor
        );

        line.drawWuLine(
            minX, maxY,
            minX, minY,
            selectionColor
        );

        return;
    }

    Circle* selectedCircle =
        dynamic_cast<Circle*>(selectedShape);

    if (selectedCircle != nullptr)
    {
        Point center = selectedCircle->getXy();
        double radius = selectedCircle->getRadius();

        int minX = center.getX() - radius - 5;
        int maxX = center.getX() + radius + 5;

        int minY = center.getY() - radius - 5;
        int maxY = center.getY() + radius + 5;

        line.drawWuLine(
            minX, minY,
            maxX, minY,
            selectionColor
        );

        line.drawWuLine(
            maxX, minY,
            maxX, maxY,
            selectionColor
        );

        line.drawWuLine(
            maxX, maxY,
            minX, maxY,
            selectionColor
        );

        line.drawWuLine(
            minX, maxY,
            minX, minY,
            selectionColor
        );

        return;
    }

    Rectangle* selectedRectangle =
        dynamic_cast<Rectangle*>(selectedShape);

    if (selectedRectangle != nullptr)
    {
        Point p = selectedRectangle->getXy();

        int minX = p.getX() - 5;
        int minY = p.getY() - 5;

        int maxX =
            p.getX() +
            selectedRectangle->getWidth() +
            5;

        int maxY =
            p.getY() +
            selectedRectangle->getHeight() +
            5;

        line.drawWuLine(
            minX, minY,
            maxX, minY,
            selectionColor
        );

        line.drawWuLine(
            maxX, minY,
            maxX, maxY,
            selectionColor
        );

        line.drawWuLine(
            maxX, maxY,
            minX, maxY,
            selectionColor
        );

        line.drawWuLine(
            minX, maxY,
            minX, minY,
            selectionColor
        );

        return;
    }
    Polygon* selectedPolygon =
        dynamic_cast<Polygon*>(selectedShape);

    if (selectedPolygon != nullptr)
    {
        list<Point> points =
            selectedPolygon->getPontos();

        if (points.empty())
        {
            return;
        }

        int minX = points.front().getX();
        int maxX = points.front().getX();

        int minY = points.front().getY();
        int maxY = points.front().getY();

        for (Point p : points)
        {
            if (p.getX() < minX)
                minX = p.getX();

            if (p.getX() > maxX)
                maxX = p.getX();

            if (p.getY() < minY)
                minY = p.getY();

            if (p.getY() > maxY)
                maxY = p.getY();
        }

        minX -= 5;
        maxX += 5;
        minY -= 5;
        maxY += 5;

        line.drawWuLine(
            minX, minY,
            maxX, minY,
            selectionColor
        );

        line.drawWuLine(
            maxX, minY,
            maxX, maxY,
            selectionColor
        );

        line.drawWuLine(
            maxX, maxY,
            minX, maxY,
            selectionColor
        );

        line.drawWuLine(
            minX, maxY,
            minX, minY,
            selectionColor
        );

        return;
    }
    Curve* selectedCurve =
        dynamic_cast<Curve*>(selectedShape);

    if (selectedCurve != nullptr)
    {
        Point p = selectedCurve->getPoint(0);

        int minX = p.getX();
        int maxX = p.getX();

        int minY = p.getY();
        int maxY = p.getY();

        for (int i = 1; i < 4; i++)
        {
            Point point =
                selectedCurve->getPoint(i);

            if (point.getX() < minX)
                minX = point.getX();

            if (point.getX() > maxX)
                maxX = point.getX();

            if (point.getY() < minY)
                minY = point.getY();

            if (point.getY() > maxY)
                maxY = point.getY();
        }

        minX -= 5;
        maxX += 5;
        minY -= 5;
        maxY += 5;

        line.drawWuLine(
            minX, minY,
            maxX, minY,
            selectionColor
        );

        line.drawWuLine(
            maxX, minY,
            maxX, maxY,
            selectionColor
        );

        line.drawWuLine(
            maxX, maxY,
            minX, maxY,
            selectionColor
        );

        line.drawWuLine(
            minX, maxY,
            minX, minY,
            selectionColor
        );

        return;
    }
}
