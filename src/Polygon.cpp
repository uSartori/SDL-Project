#include "Polygon.h"
#include "Context.h"
#include "Line.h"
#include "Transform.h"
#include <algorithm>

// Construtor vazio
Polygon::Polygon()
{
}

// Destrutor
Polygon::~Polygon()
{
}

// Cria um poligono
Polygon::Polygon(list<Point> pontos, Color color)
{
    this->pontos = pontos;
    this->color = color;
}

// Retorna os pontos
list<Point> Polygon::getPontos()
{
    return pontos;
}

// Retorna a cor
Color Polygon::getColor()
{
    return color;
}

// Altera os pontos
void Polygon::setPontos(list<Point> pontos)
{
    this->pontos = pontos;
}

// Altera a cor
void Polygon::setColor(Color color)
{
    this->color = color;
}

// Desenha o poligono ligando os pontos
void Polygon::drawPolygon(list<Point> pontos, Color cor)
{
    // Um polígono precisa de pelo menos dois pontos para formar uma linha
    if (pontos.size() < 2)
        return;

    // Guarda o primeiro ponto porque no final precisamos ligar o último ponto de volta nele
    Point primeiro = pontos.front();
    Point anterior = primeiro;
    Point atual;

    int i = 0;

    // Percorre todos os pontos da lista para desenhar cada lado do polígono
    for (Point ponto : pontos)
    {
        // O primeiro ponto só é armazenado, porque ainda não existe um ponto anterior para formar uma linha
        if (i > 0)
        {
            atual = ponto;

            Line line;
            // Usa o algoritmo de Xiaolin Wu para desenhar cada lado do polígono com antialiasing
            line.drawWuLine(anterior.getX(), anterior.getY(), atual.getX(), atual.getY(), cor);

            // O ponto atual passa a ser o anterior para desenhar o próximo lado
            anterior = atual;
        }

        i++;
    }

    Line line;
    // Depois de desenhar todos os lados, liga o último ponto ao primeiro para fechar o polígono
    line.drawWuLine(atual.getX(), atual.getY(), primeiro.getX(), primeiro.getY(), cor);
}

// Desenha o poligono
void Polygon::draw()
{
    drawPolygon(pontos, color);
}

// Translada os pontos do poligono
void Polygon::translate(double tx, double ty)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();
    // Cria um vetor temporário porque o Transform trabalha com um array de Point
    Point* points = new Point[numPoints];

    int i = 0;

    // Copia os pontos da lista para o array
    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    // Aplica a translação em todos os pontos do polígono
    transform.translate(points, numPoints, tx, ty);

    // Remove os pontos antigos para colocar os pontos já transformados
    pontos.clear();

    // Copia os pontos transformados de volta para a lista
    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    // Libera o array temporário criado com new
    delete[] points;
}

// Escala o poligono usando um ponto de referencia
void Polygon::scale(double sx, double sy, Point reference)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();
    // Cria um array temporário para poder usar a função de escala do Transform
    Point* points = new Point[numPoints];

    int i = 0;

    // Copia os pontos da lista para o array
    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    // Aplica a escala usando o ponto de referência recebido
    // O ponto de referência é importante porque define qual parte do polígono permanece fixa durante a escala
    transform.scale(points, numPoints, sx, sy, reference);

    // Remove os pontos antigos
    pontos.clear();

    // Coloca os pontos já escalados novamente na lista
    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    // Libera a memória do array temporário
    delete[] points;
}

// Rotaciona o poligono usando um ponto de referencia
void Polygon::rotate(double angle, Point reference)
{
    if (pontos.empty())
        return;

    int numPoints = pontos.size();

    // Cria um array temporário porque o Transform recebe os pontos dessa forma
    Point* points = new Point[numPoints];

    int i = 0;

    // Copia os pontos atuais do polígono para o array
    for (Point point : pontos)
        points[i++] = point;

    Transform transform;
    // Rotaciona todos os pontos em torno do ponto de referência
    transform.rotate(points, numPoints, angle, reference);

    // Remove os pontos antigos antes de colocar os pontos rotacionados
    pontos.clear();

    // Copia os pontos transformados novamente para a lista
    for (i = 0; i < numPoints; i++)
        pontos.push_back(points[i]);

    // Libera o array temporário
    delete[] points;
}

// Verifica se um ponto está perto de um segmento de reta
bool Polygon::isPointNearLine(int clickX, int clickY, Point a, Point b)
{
    double dx = b.getX() - a.getX();
    double dy = b.getY() - a.getY();

    // Calcula o comprimento do segmento ao quadrado
    double lengthSquared = dx * dx + dy * dy;

    // Se os dois pontos forem iguais, verifica a distância diretamente até o ponto
    if (lengthSquared == 0)
    {
        double px = clickX - a.getX();
        double py = clickY - a.getY();

        // Aceita uma distância de até 5 pixels
        return px * px + py * py <= 25;
    }

    // Calcula a posição do ponto mais próximo dentro do segmento
    double t = ((clickX - a.getX()) * dx + (clickY - a.getY()) * dy) / lengthSquared;

    // Garante que o ponto encontrado fique entre a e b
    t = std::max(0.0, std::min(1.0, t));

    // Calcula as coordenadas do ponto mais próximo
    double closestX = a.getX() + t * dx;
    double closestY = a.getY() + t * dy;

    // Calcula a distância entre o clique e o ponto mais próximo
    double distanceX = clickX - closestX;
    double distanceY = clickY - closestY;

    // Verifica se a distância é de no máximo 5 pixels
    return distanceX * distanceX + distanceY * distanceY <= 25;
}

// Verifica se o clique está perto do poligono
bool Polygon::isNear(int clickX, int clickY)
{
    if (pontos.size() < 2)
        return false;

    // Guarda o primeiro ponto porque o último lado precisa voltar para ele
    Point first = pontos.front();
    Point previous = first;

    // Percorre os pontos verificando cada lado do polígono
    for (auto it = std::next(pontos.begin()); it != pontos.end(); ++it)
    {
        // Verifica se o clique está próximo do lado atual
        if (isPointNearLine(clickX, clickY, previous, *it))
            return true;

        previous = *it;
    }

    // Verifica o último lado, que liga o último ponto ao primeiro
    return isPointNearLine(clickX, clickY, previous, first);
}
