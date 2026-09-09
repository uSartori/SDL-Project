#ifndef SHAPE_H
#define SHAPE_H

#include "Point.h"

// Classe base para todas as figuras geométricas do projeto (funciona como uma interface em java).
class Shape
{
public:

    // O "= 0" torna obrigatorio essas operacoes para cada figura
    virtual void draw() = 0;

    // Move a figura para outra posição.
    virtual void translate(double tx, double ty) = 0;

    // Altera o tamanho da figura.
    virtual void scale( double sx, double sy, Point reference) = 0;

    // Rotaciona a figura pelo ângulo informado em graus.
    virtual void rotate(double angle, Point reference) = 0;

    // Verifica se o click do mouse foi proximo da figura
    virtual bool isNear(int x, int y) = 0;

    virtual ~Shape() = default;
};

#endif // SHAPE_H
