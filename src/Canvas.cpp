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
    // O Canvas começa em y = 40 porque os primeiros 40 pixels são usados pela barra de ferramentas
    x = 0;
    y = 40;
    // Nenhuma figura começa selecionada
    selectedShape = nullptr;
}

// Libera as figuras
Canvas::~Canvas()
{
    // As figuras foram criadas com new, então precisam ser deletadas para não deixar memória ocupada
    for (Shape* shape : shapes)
        delete shape;
}

// Adiciona uma figura
void Canvas::addShape(Shape* shape)
{
    // Verifica se a figura existe antes de adicionar no vetor
    if (shape != nullptr)
        shapes.push_back(shape);
}

// Desenha o Canvas
void Canvas::draw()
{
    // Desenha todas as figuras que estão armazenadas no Canvas
    for (Shape* shape : shapes)
        if (shape != nullptr)
            shape->draw();

    FloodFill floodFill;

    // Aplica novamente os Flood Fills que foram armazenados
    // Isso é necessário porque o Canvas é redesenhado várias vezes durante a execução
    for (const Fill& fill : fills)
        floodFill.fill(fill.point, fill.color);

    // Se existe uma figura selecionada, desenha a caixa e os handles
    if (selectedShape != nullptr)
    {
        drawSelectionIndicator();
        drawSelectionHandles();
    }
}

// Limpa o Canvas
void Canvas::clear()
{
    // Define o viewport para a área do Canvas para não apagar a barra de ferramentas
    Context::getInstance()->setViewport(x, y, width, height);

    Line line;

    // Percorre todos os pixels do Canvas e pinta cada um de branco
    for (int px = x; px < x + width; px++)
        for (int py = y; py < y + height; py++)
            line.setPixel(px, py, 255, 255, 255);
}

// Verifica os limites
bool Canvas::isInside(int px, int py) const
{
    // Verifica se o ponto está dentro dos limites do Canvas
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
    // Só guarda o preenchimento se o ponto inicial estiver dentro do Canvas
    if (isInside(startPoint.getX(), startPoint.getY()))
        fills.push_back({startPoint, newColor});
}

// Calcula os limites da figura
bool Canvas::getShapeBounds(Shape* shape, int& minX, int& minY, int& maxX, int& maxY)
{
    if (shape == nullptr)
        return false;

    // Começa com valores extremos para conseguir encontrar os menores e maiores valores dos pontos
    minX = minY = 999999;
    maxX = maxY = -999999;

    // Verifica se a figura é uma Line
    // O dynamic_cast é usado porque o vetor guarda todas as figuras como Shape*
    if (Line* line = dynamic_cast<Line*>(shape))
    {
        Point p1 = line->getStart();
        Point p2 = line->getEnd();

        // Como uma reta possui dois pontos, os limites são encontrados comparando as coordenadas deles
        minX = std::min(p1.getX(), p2.getX());
        minY = std::min(p1.getY(), p2.getY());
        maxX = std::max(p1.getX(), p2.getX());
        maxY = std::max(p1.getY(), p2.getY());

        return true;
    }

    // Verifica se a figura é um Circle
    if (Circle* circle = dynamic_cast<Circle*>(shape))
    {
        Point c = circle->getXy();
        double r = circle->getRadius();

        // O limite do círculo é encontrado somando e subtraindo o raio do centro
        minX = c.getX() - r;
        minY = c.getY() - r;
        maxX = c.getX() + r;
        maxY = c.getY() + r;

        return true;
    }

    // Verifica se a figura é um Rectangle
    if (Rectangle* rect = dynamic_cast<Rectangle*>(shape))
    {
        Point p = rect->getXy();

        // Calcula o centro do retângulo, que também será o centro usado na rotação
        double cx = p.getX() + rect->getWidth() / 2.0;
        double cy = p.getY() + rect->getHeight() / 2.0;

        // Converte o ângulo de graus para radianos porque sin e cos usam radianos
        double a = rect->getRotation() * M_PI / 180.0;
        double cosA = cos(a), sinA = sin(a);

        // Guarda os quatro cantos do retângulo antes da rotação
        Point corners[4] = {
            Point(p.getX(), p.getY()),
            Point(p.getX() + rect->getWidth(), p.getY()),
            Point(p.getX() + rect->getWidth(), p.getY() + rect->getHeight()),
            Point(p.getX(), p.getY() + rect->getHeight())
        };

        for (Point corner : corners)
        {
            // Calcula a posição do canto em relação ao centro do retângulo
            double dx = corner.getX() - cx;
            double dy = corner.getY() - cy;

            // Aplica a fórmula de rotação para descobrir onde o canto ficou depois da rotação
            int rx = (int)(dx * cosA - dy * sinA + cx);
            int ry = (int)(dx * sinA + dy * cosA + cy);

            // Compara cada canto com os limites encontrados até agora
            // Assim conseguimos criar uma caixa que envolve o retângulo mesmo quando ele está rotacionado
            minX = std::min(minX, rx);
            minY = std::min(minY, ry);
            maxX = std::max(maxX, rx);
            maxY = std::max(maxY, ry);
        }

        return true;
    }

    // Verifica se a figura é um Polygon
    if (Polygon* polygon = dynamic_cast<Polygon*>(shape))
    {
        std::list<Point> points = polygon->getPontos();

        if (points.empty())
            return false;

        // Percorre todos os pontos para encontrar os limites do polígono
        for (Point p : points)
        {
            minX = std::min(minX, p.getX());
            minY = std::min(minY, p.getY());
            maxX = std::max(maxX, p.getX());
            maxY = std::max(maxY, p.getY());
        }

        return true;
    }

    // Verifica se a figura é uma Curve
    if (Curve* curve = dynamic_cast<Curve*>(shape))
    {
        // A curva possui quatro pontos de controle que são usados para encontrar seus limites
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

// Calcula a distância de um ponto até um segmento de reta
double Canvas::distanceToSegment(int px, int py, Point p1, Point p2)
{
    // Calcula o vetor que representa o segmento
    double dx = p2.getX() - p1.getX();
    double dy = p2.getY() - p1.getY();

    // Calcula o tamanho ao quadrado do segmento
    double length = dx * dx + dy * dy;

    // Calcula a posição do ponto projetado sobre a reta usando o valor t
    // t = 0 representa o começo e t = 1 representa o final do segmento
    double t = length == 0 ? 0 :
        ((px - p1.getX()) * dx + (py - p1.getY()) * dy) / length;

    // Garante que o ponto projetado fique dentro do segmento e não na extensão da reta
    t = std::max(0.0, std::min(1.0, t));

    // Encontra o ponto do segmento que está mais próximo do ponto recebido
    double cx = p1.getX() + t * dx;
    double cy = p1.getY() + t * dy;

    // Calcula a diferença entre o ponto recebido e o ponto mais próximo do segmento
    double ex = px - cx;
    double ey = py - cy;

    // Calcula a distância usando a fórmula da distância euclidiana
    return sqrt(ex * ex + ey * ey);
}

// Verifica se um ponto está próximo da figura
bool Canvas::isPointNearShape(Shape* shape, int px, int py, double tolerance)
{
    if (shape == nullptr)
        return false;

    // Para uma reta, verifica a distância do ponto até o segmento
    // A tolerância permite clicar alguns pixels ao lado da linha e ainda selecioná-la
    if (Line* line = dynamic_cast<Line*>(shape))
        return distanceToSegment(px, py, line->getStart(), line->getEnd()) <= tolerance;

    // Verifica a distância até a borda do círculo
    if (Circle* circle = dynamic_cast<Circle*>(shape))
    {
        Point c = circle->getXy();

        // Calcula a distância do ponto do mouse até o centro do círculo
        double dx = px - c.getX();
        double dy = py - c.getY();

        // Se a distância até o centro for igual ao raio, o ponto está na borda
        // O fabs serve para aceitar uma pequena diferença para dentro ou para fora do círculo
        return fabs(sqrt(dx * dx + dy * dy) - circle->getRadius()) <= tolerance;
    }

    // Verifica os lados do polígono
    if (Polygon* polygon = dynamic_cast<Polygon*>(shape))
    {
        std::list<Point> points = polygon->getPontos();

        if (points.size() < 2)
            return false;

        // Começa pelo último ponto para fechar o polígono com o primeiro ponto
        Point prev = points.back();

        for (Point cur : points)
        {
            // Verifica se o ponto está próximo do lado atual
            if (distanceToSegment(px, py, prev, cur) <= tolerance)
                return true;

            prev = cur;
        }

        return false;
    }

    // Verifica a proximidade com uma curva de Bézier
    if (Curve* curve = dynamic_cast<Curve*>(shape))
    {
        Point prev = curve->getPoint(0);

        // A curva não é formada diretamente por segmentos de reta
        // Então ela é dividida em vários pequenos segmentos para facilitar a verificação
        for (double t = 0.001; t <= 1.0; t += 0.001)
        {
            double mt = 1 - t;

            // Calcula a posição X da curva usando a fórmula da Bézier cúbica
            double x =
                mt * mt * mt * curve->getPoint(0).getX() +
                3 * mt * mt * t * curve->getPoint(1).getX() +
                3 * mt * t * t * curve->getPoint(2).getX() +
                t * t * t * curve->getPoint(3).getX();

            // Calcula a posição Y da curva usando a mesma fórmula
            double y =
                mt * mt * mt * curve->getPoint(0).getY() +
                3 * mt * mt * t * curve->getPoint(1).getY() +
                3 * mt * t * t * curve->getPoint(2).getY() +
                t * t * t * curve->getPoint(3).getY();

            Point cur((int)x, (int)y);

            // Verifica a distância do mouse até o pequeno segmento atual da curva
            if (distanceToSegment(px, py, prev, cur) <= tolerance)
                return true;

            prev = cur;
        }

        return false;
    }

    // Verifica a proximidade com um retângulo
    if (Rectangle* rect = dynamic_cast<Rectangle*>(shape))
    {
        Point p = rect->getXy();

        // Calcula o centro do retângulo
        double cx = p.getX() + rect->getWidth() / 2.0;
        double cy = p.getY() + rect->getHeight() / 2.0;

        // Usa o ângulo negativo para desfazer a rotação do retângulo
        // Assim o ponto do mouse pode ser analisado como se o retângulo não estivesse rotacionado
        double a = -rect->getRotation() * M_PI / 180.0;

        double dx = px - cx;
        double dy = py - cy;

        // Rotaciona o ponto do mouse no sentido contrário ao retângulo
        double lx = dx * cos(a) - dy * sin(a) + cx;
        double ly = dx * sin(a) + dy * cos(a) + cy;

        // Calcula os quatro limites do retângulo original
        int left = p.getX();
        int right = left + rect->getWidth();
        int top = p.getY();
        int bottom = top + rect->getHeight();

        // Verifica a distância do ponto até cada um dos quatro lados
        // Se estiver perto de qualquer lado, considera que o retângulo foi clicado
        return distanceToSegment(lx, ly, Point(left, top), Point(right, top)) <= tolerance ||
               distanceToSegment(lx, ly, Point(right, top), Point(right, bottom)) <= tolerance ||
               distanceToSegment(lx, ly, Point(right, bottom), Point(left, bottom)) <= tolerance ||
               distanceToSegment(lx, ly, Point(left, bottom), Point(left, top)) <= tolerance;
    }

    return false;
}

// Seleciona a figura clicada
void Canvas::selectShape(int px, int py)
{
    // Remove a seleção anterior antes de procurar uma nova figura
    selectedShape = nullptr;

    if (!isInside(px, py))
        return;

    // Percorre as figuras de trás para frente
    // Isso faz com que a figura desenhada por último seja selecionada primeiro quando existe sobreposição
    for (int i = shapes.size() - 1; i >= 0; i--)
        if (isPointNearShape(shapes[i], px, py, 5))
        {
            selectedShape = shapes[i];
            return;
        }
}

// Desenha a caixa delimitadora
void Canvas::drawBoundingBox(int minX, int minY, int maxX, int maxY, Color color)
{
    // Cria um retângulo usando os limites encontrados da figura
    Rectangle(Point(minX, minY), maxX - minX, maxY - minY, color).draw();
}

// Desenha o indicador de seleção
void Canvas::drawSelectionIndicator()
{
    int minX, minY, maxX, maxY;

    // Calcula os limites da figura e aumenta 5 pixels em cada lado
    // para que a caixa fique um pouco afastada da figura
    if (selectedShape != nullptr &&
        getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        drawBoundingBox(minX - 5, minY - 5, maxX + 5, maxY + 5, Color(0, 0, 255));
}

// Desenha um handle
void Canvas::drawHandle(int x, int y, Color color)
{
    Line line;

    // Desenha um pequeno quadrado de 11 por 11 pixels no ponto recebido
    // Esse quadrado representa um ponto de manipulação da figura
    for (int px = x - 5; px <= x + 5; px++)
        for (int py = y - 5; py <= y + 5; py++)
            line.setPixel(px, py, color);
}

// Desenha os handles da seleção
void Canvas::drawSelectionHandles()
{
    int minX, minY, maxX, maxY;

    if (selectedShape == nullptr ||
        !getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    Color blue(0, 0, 255);

    // Calcula o centro horizontal da caixa
    int cx = (minX + maxX) / 2;

    // Handles dos quatro cantos usados para redimensionar
    drawHandle(minX, minY, blue);
    drawHandle(maxX, minY, blue);
    drawHandle(minX, maxY, blue);
    drawHandle(maxX, maxY, blue);

    // Handle acima da figura usado para rotação
    drawHandle(cx, minY - 25, blue);

    // Handle abaixo da figura usado para movimentação
    drawHandle(cx, maxY + 25, blue);
}

// Detecta qual handle está sendo clicado
Canvas::HandleType Canvas::getHandleAt(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return HANDLE_NONE;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return HANDLE_NONE;

    // Define uma tolerância para facilitar o clique nos handles
    int t = 8;

    // Calcula o centro da caixa
    int cx = (minX + maxX) / 2;

    // Verifica os quatro handles usados para escala
    if (abs(mouseX - minX) <= t && abs(mouseY - minY) <= t) return HANDLE_SCALE_TOP_LEFT;
    if (abs(mouseX - maxX) <= t && abs(mouseY - minY) <= t) return HANDLE_SCALE_TOP_RIGHT;
    if (abs(mouseX - minX) <= t && abs(mouseY - maxY) <= t) return HANDLE_SCALE_BOTTOM_LEFT;
    if (abs(mouseX - maxX) <= t && abs(mouseY - maxY) <= t) return HANDLE_SCALE_BOTTOM_RIGHT;

    // O handle de rotação fica 25 pixels acima da caixa
    if (abs(mouseX - cx) <= t && abs(mouseY - minY + 25) <= t)
        return HANDLE_ROTATE;

    // O handle de movimentação fica 25 pixels abaixo da caixa
    if (abs(mouseX - cx) <= t && abs(mouseY - maxY - 25) <= t)
        return HANDLE_TRANSLATE;

    return HANDLE_NONE;
}

// Inicia a escala da figura
void Canvas::startScale(HandleType handle, int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    if (Rectangle* rect = dynamic_cast<Rectangle*>(selectedShape))
    {
        originalRectXy = rect->getXy();
        originalRectWidth = rect->getWidth();
        originalRectHeight = rect->getHeight();
        originalRectRotation = rect->getRotation();
    }

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    // Limpa os pontos armazenados de uma escala anterior
    originalScalePoints.clear();
    originalCurvePoints.clear();

    // Guarda os pontos originais do polígono
    // Isso evita que uma nova escala seja aplicada sobre a escala anterior
    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
        for (Point p : polygon->getPontos())
            originalScalePoints.push_back(p);

    // Guarda os pontos originais da curva pelo mesmo motivo
    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
        for (int i = 0; i < 4; i++)
            originalCurvePoints.push_back(curve->getPoint(i));

    // Guarda o tamanho original da figura
    originalWidth = maxX - minX;
    originalHeight = maxY - minY;

    // Guarda a posição inicial do mouse
    scaleStartMouse = Point(mouseX, mouseY);

    // O canto oposto ao handle arrastado será usado como ponto fixo da escala
    // Dessa forma, um canto permanece parado enquanto o outro é movimentado
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

    // Calcula a nova largura e altura de acordo com o canto que está sendo arrastado
    if (handle == HANDLE_SCALE_TOP_LEFT) { w = scaleReference.getX() - mouseX; h = scaleReference.getY() - mouseY; }
    if (handle == HANDLE_SCALE_TOP_RIGHT) { w = mouseX - scaleReference.getX(); h = scaleReference.getY() - mouseY; }
    if (handle == HANDLE_SCALE_BOTTOM_LEFT) { w = scaleReference.getX() - mouseX; h = mouseY - scaleReference.getY(); }
    if (handle == HANDLE_SCALE_BOTTOM_RIGHT) { w = mouseX - scaleReference.getX(); h = mouseY - scaleReference.getY(); }

    // Impede que a largura ou altura fiquem negativas
    if (w <= 0 || h <= 0)
        return;

    // Define o tamanho mínimo que a figura pode ter
    w = std::max(12.0, w);
    h = std::max(12.0, h);

    // Calcula quanto a figura precisa aumentar ou diminuir em relação ao tamanho original
    double sx = w / originalWidth;
    double sy = h / originalHeight;

    // Se a figura selecionada for um Retângulo:
    if (Rectangle* rect = dynamic_cast<Rectangle*>(selectedShape))
    {
        // 1. Restaura o estado original capturado no startScale
        rect->setXy(originalRectXy);
        rect->setWidth(originalRectWidth);
        rect->setHeight(originalRectHeight);

        // 2. Aplica a nova escala a partir do estado original fixo
        rect->scale(sx, sy, scaleReference);
        return;
    }

    // Polígonos precisam voltar aos pontos originais antes de aplicar a nova escala
    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
    {
        std::list<Point> points(originalScalePoints.begin(), originalScalePoints.end());
        polygon->setPontos(points);
        polygon->scale(sx, sy, scaleReference);
        return;
    }

    // Faz o mesmo processo para a curva
    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
    {
        if (originalCurvePoints.size() != 4)
            return;

        for (int i = 0; i < 4; i++)
            curve->setPoint(i, originalCurvePoints[i]);

        curve->scale(sx, sy, scaleReference);
        return;
    }

    // Código fallback para outras formas
    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    double cw = maxX - minX;
    double ch = maxY - minY;

    if (cw <= 0 || ch <= 0)
        return;

    selectedShape->scale(w / cw, h / ch, scaleReference);
}

// Deleta a figura selecionada
void Canvas::deleteSelectedShape()
{
    // Procura a figura selecionada dentro do vetor
    auto it = std::find(shapes.begin(), shapes.end(), selectedShape);

    if (it == shapes.end())
        return;

    // Libera a memória da figura
    delete *it;

    // Remove a figura do vetor
    shapes.erase(it);

    // Remove a seleção
    selectedShape = nullptr;
}

// Limpa a seleção
void Canvas::clearSelection()
{
    selectedShape = nullptr;
}

// Inicia a rotação
void Canvas::startRotate(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    int minX, minY, maxX, maxY;

    if (!getShapeBounds(selectedShape, minX, minY, maxX, maxY))
        return;

    // O centro da rotação é o centro da caixa que envolve a figura
    rotationCenter = Point((minX + maxX) / 2, (minY + maxY) / 2);

    // Guarda a posição inicial do mouse para calcular o quanto ele se movimentou
    rotationStartMouseX = mouseX;

    // Começa a rotação com ângulo zero
    rotationTotalAngle = 0;

    // Limpa os pontos usados em uma rotação anterior
    originalRotationPoints.clear();

    // Uma reta possui dois pontos, então guarda os dois pontos originais
    // Eles serão usados como base para cada nova atualização da rotação
    if (Line* line = dynamic_cast<Line*>(selectedShape))
    {
        originalRotationStart = line->getStart();
        originalRotationEnd = line->getEnd();
        return;
    }

    // Guarda todos os pontos originais do polígono
    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
    {
        for (Point p : polygon->getPontos())
            originalRotationPoints.push_back(p);

        return;
    }

    // Guarda os quatro pontos de controle da curva
    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
        for (int i = 0; i < 4; i++)
            originalRotationPoints.push_back(curve->getPoint(i));
}

// Atualiza a rotação
void Canvas::updateRotate(int mouseX, int mouseY)
{
    if (selectedShape == nullptr)
        return;

    // Transforma o movimento horizontal do mouse em um ângulo
    // O 0.4 controla a velocidade da rotação
    rotationTotalAngle = (mouseX - rotationStartMouseX) * 0.4;

    Transform transform;

    // Rotação de uma reta
    if (Line* line = dynamic_cast<Line*>(selectedShape))
    {
        // Cria uma cópia dos pontos originais para aplicar a transformação
        Point points[2] = {originalRotationStart, originalRotationEnd};

        // Rotaciona os dois pontos em torno do centro da figura
        transform.rotate(points, 2, rotationTotalAngle, rotationCenter);

        // Atualiza os pontos da reta
        line->setStart(points[0]);
        line->setEnd(points[1]);

        return;
    }

    // Rotação de um polígono
    if (Polygon* polygon = dynamic_cast<Polygon*>(selectedShape))
    {
        if (originalRotationPoints.empty())
            return;

        // Copia os pontos originais para não acumular a rotação a cada movimento do mouse
        std::vector<Point> points = originalRotationPoints;

        // Aplica a rotação em todos os pontos do polígono
        transform.rotate(points.data(), points.size(), rotationTotalAngle, rotationCenter);

        // Converte novamente para list porque o Polygon usa uma lista de pontos
        std::list<Point> result(points.begin(), points.end());

        polygon->setPontos(result);
        return;
    }

    // Rotação de uma curva
    if (Curve* curve = dynamic_cast<Curve*>(selectedShape))
    {
        if (originalRotationPoints.size() != 4)
            return;

        Point points[4];

        // Copia os quatro pontos originais da curva
        for (int i = 0; i < 4; i++)
            points[i] = originalRotationPoints[i];

        // Rotaciona os quatro pontos em torno do centro
        transform.rotate(points, 4, rotationTotalAngle, rotationCenter);

        // Atualiza os pontos da curva com as novas posições
        for (int i = 0; i < 4; i++)
            curve->setPoint(i, points[i]);

        return;
    }

    // Para as outras figuras, usa a própria função virtual de rotação da figura
    // Isso permite que cada classe cuide da sua própria forma de aplicar a rotação
    selectedShape->rotate(rotationTotalAngle, rotationCenter);
}
