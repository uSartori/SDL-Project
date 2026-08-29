#ifndef SHAPE_H
#define SHAPE_H

// Classe base para todas as figuras geométricas do projeto (funciona como uma interface em java).
//
// Como cada figura do projeto é diferente, cada figura deve implementar essas operacoes de acordo com sua necessidade.
class Shape
{
public:

    // O "= 0" torna obrigatorio essas operacoes para cada figura
    virtual void draw() = 0;

    // Move a figura para outra posição.
    virtual void translate(double tx, double ty) = 0;

    // Altera o tamanho da figura.
    virtual void scale(double sx, double sy) = 0;

    // Rotaciona a figura pelo ângulo informado em graus.
    virtual void rotate(double angle) = 0;

    virtual ~Shape() = default;
};

#endif // SHAPE_H
