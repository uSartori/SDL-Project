#include "Rectangle.h"
#include "Line.h"
#include "Transform.h"
#include <Color.h>
#include <cmath>
#include <algorithm>

// Construtor vazio
Rectangle::Rectangle()
{
}

// Destrutor
Rectangle::~Rectangle()
{
}

// Cria um retangulo a partir de um ponto e suas dimensoes
Rectangle::Rectangle(Point xy, double width, double height, Color color)
{
    this->xy = xy;
    this->width = width;
    this->height = height;
    this->color = color;
    this->rotation = 0;
}

// Cria um retangulo usando dois pontos
Rectangle::Rectangle(Point xy0, Point xy1, Color color)
{
    this->xy = xy0;
    this->width = xy1.getX() - xy0.getX();
    this->height = xy1.getY() - xy0.getY();
    this->color = color;
    this->rotation = 0;
}

// Retorna a posição do retangulo
Point Rectangle::getXy()
{
    return xy;
}

// Retorna a largura
double Rectangle::getWidth()
{
    return width;
}

// Retorna a altura
double Rectangle::getHeight()
{
    return height;
}

// Altera a posição
void Rectangle::setXy(Point xy)
{
    this->xy = xy;
}

// Altera a largura
void Rectangle::setWidth(double width)
{
    this->width = width;
}

// Altera a altura
void Rectangle::setHeight(double height)
{
    this->height = height;
}

// Desenha os quatro lados do retangulo
void Rectangle::draw()
{
    Point p1 = xy;
    Point p2(xy.getX() + width, xy.getY());
    Point p3(xy.getX() + width, xy.getY() + height);
    Point p4(xy.getX(), xy.getY() + height);

    Point points[4] = {p1, p2, p3, p4};

    Point center(xy.getX() + width / 2, xy.getY() + height / 2);

    if (rotation != 0)
    {
        Transform transform;
        transform.rotate(points, 4, rotation, center);
    }

    Line line;

    line.drawWuLine(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY(), color);
    line.drawWuLine(points[1].getX(), points[1].getY(), points[2].getX(), points[2].getY(), color);
    line.drawWuLine(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY(), color);
    line.drawWuLine(points[3].getX(), points[3].getY(), points[0].getX(), points[0].getY(), color);
}

// Translada o retangulo
void Rectangle::translate(double tx, double ty)
{
    xy.setX(xy.getX() + tx);
    xy.setY(xy.getY() + ty);
}

// Escala o retangulo considerando sua rotação atual
void Rectangle::scale(double sx, double sy, Point reference)
{
    // Aplica a escala nas dimensões locais
    width *= sx;
    height *= sy;

    // Calcula o vetor de translação da origem 'xy' em relação à referência
    double dx = xy.getX() - reference.getX();
    double dy = xy.getY() - reference.getY();

    // Atualiza a posição mantendo o correto deslocamento
    xy.setX(reference.getX() + dx * sx);
    xy.setY(reference.getY() + dy * sy);
}
/*void Rectangle::scale(double sx, double sy, Point reference)
{
    Point points[4] = {
        Point(xy.getX(), xy.getY()),
        Point(xy.getX() + width, xy.getY()),
        Point(xy.getX() + width, xy.getY() + height),
        Point(xy.getX(), xy.getY() + height)
    };

    Transform transform;
    transform.scale(points, 4, sx, sy, reference);

    int minX = points[0].getX();
    int minY = points[0].getY();
    int maxX = points[0].getX();
    int maxY = points[0].getY();

    for (int i = 1; i < 4; i++)
    {
        minX = std::min(minX, points[i].getX());
        minY = std::min(minY, points[i].getY());
        maxX = std::max(maxX, points[i].getX());
        maxY = std::max(maxY, points[i].getY());
    }

    xy = Point(minX, minY);
    width = maxX - minX;
    height = maxY - minY;
}*/

// Rotaciona o retangulo
// Rotaciona o retangulo em torno do seu centro
void Rectangle::rotate(double angle, Point reference)
{
    rotation += angle * 0.2;
}
// Verifica se um ponto está próximo de um segmento de reta
bool Rectangle::isPointNearLine(int clickX, int clickY, Point a, Point b)
{
    // Calcula o vetor que representa o segmento
    double dx = b.getX() - a.getX();
    double dy = b.getY() - a.getY();

    // Calcula o comprimento do segmento ao quadrado
    double lengthSquared = dx * dx + dy * dy;

    // Verifica se os dois pontos são iguais
    if (lengthSquared == 0)
    {
        // Calcula a diferença entre o clique e o ponto
        double px = clickX - a.getX();
        double py = clickY - a.getY();

        // Aceita o clique se estiver a no máximo 5 pixels do ponto
        return px * px + py * py <= 25;
    }

    // Calcula a posição do ponto mais próximo do clique sobre o segmento
    double t = ((clickX - a.getX()) * dx + (clickY - a.getY()) * dy) / lengthSquared;

    // Limita o valor entre 0 e 1 para considerar somente o segmento
    t = std::max(0.0, std::min(1.0, t));

    // Calcula as coordenadas do ponto mais próximo do clique
    double closestX = a.getX() + t * dx;
    double closestY = a.getY() + t * dy;

    // Calcula a diferença entre o clique e o ponto mais próximo
    double distanceX = clickX - closestX;
    double distanceY = clickY - closestY;

    // Verifica se o clique está a até 5 pixels do segmento
    return distanceX * distanceX + distanceY * distanceY <= 25;
}

// Verifica se o clique está próximo do retangulo
bool Rectangle::isNear(int clickX, int clickY)
{
    Point p = xy;

    // Centro do retângulo
    double cx = p.getX() + width / 2.0;
    double cy = p.getY() + height / 2.0;

    // Desfaz a rotação no ponto do clique para testar em espaço local
    double a = -rotation * M_PI / 180.0;

    double dx = clickX - cx;
    double dy = clickY - cy;

    double lx = dx * cos(a) - dy * sin(a) + cx;
    double ly = dx * sin(a) + dy * cos(a) + cy;

    Point p1 = xy;
    Point p2(xy.getX() + width, xy.getY());
    Point p3(xy.getX() + width, xy.getY() + height);
    Point p4(xy.getX(), xy.getY() + height);

    return isPointNearLine(lx, ly, p1, p2) ||
           isPointNearLine(lx, ly, p2, p3) ||
           isPointNearLine(lx, ly, p3, p4) ||
           isPointNearLine(lx, ly, p4, p1);
}
/*bool Rectangle::isNear(int clickX, int clickY)
{
    // Cria os quatro pontos que representam os cantos do retângulo
    Point p1 = xy;
    Point p2(xy.getX() + width, xy.getY());
    Point p3(xy.getX() + width, xy.getY() + height);
    Point p4(xy.getX(), xy.getY() + height);

    // Verifica os quatro lados do retângulo
    // Se o clique estiver próximo de qualquer lado, considera que o retângulo foi selecionado
    return isPointNearLine(clickX, clickY, p1, p2) ||
           isPointNearLine(clickX, clickY, p2, p3) ||
           isPointNearLine(clickX, clickY, p3, p4) ||
           isPointNearLine(clickX, clickY, p4, p1);
}*/

double Rectangle::getRotation() const
{
    return rotation;
}
