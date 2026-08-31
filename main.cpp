#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include <list>

#include "Context.h"
#include "Color.h"
#include "Point.h"
#include "Line.h"
#include "Circle.h"
#include "Curve.h"
#include "Polygon.h"
#include "Rectangle.h"
#include "Shape.h"
#include "Toolbar.h"
#include "Canvas.h"
#include "FloodFill.h"

// Controle da curva
Point curvePoints[4];
int curvePointCount = 0;

// Controle do poligono
list<Point> polygonPoints;

// Controle temporário do desenho da linha
bool drawing = false;
Point startPoint;
Point currentPoint;

// Desenha o conteúdo do Canvas e a interface
void display(Canvas& canvas, Toolbar& toolbar)
{
    Context* context = Context::getInstance();

    // Desenha somente dentro do Canvas
    context->setViewport(0, 40, 640, 440);

    canvas.draw();

    // Aparece a preview da linha conforme o usuario move o mouse
    if (drawing && toolbar.getCurrentTool() == TOOL_LINE)
    {
        Line preview(
            startPoint,
            currentPoint,
            Color(0, 0, 0)
        );

        preview.draw();
    }

    // Aparece a preview do retangulo conforme o usuario move o mouse
    if (drawing && toolbar.getCurrentTool() == TOOL_RECTANGLE)
    {
        Rectangle preview(
            startPoint,
            currentPoint,
            Color(0, 0, 0)
        );

        preview.draw();
    }

    // Aparece a preview do circulo conforme o usuario move o mouse
    if (drawing && toolbar.getCurrentTool() == TOOL_CIRCLE)
    {
        double dx = currentPoint.getX() - startPoint.getX();
        double dy = currentPoint.getY() - startPoint.getY();

        // O raio eh a distancia entre os dois pontos dx e dy
        double radius = sqrt(dx * dx + dy * dy);

        Circle preview(
            startPoint,
            radius,
            Color(0, 0, 0)
        );

        preview.draw();
    }

    // Mostra as linhas do poligono conforme os pontos sao adicionados
    if (toolbar.getCurrentTool() == TOOL_POLYGON &&
        polygonPoints.size() >= 2)
    {
        Line line;
        Point anterior = polygonPoints.front();

        int i = 0;

        for (Point ponto : polygonPoints)
        {
            if (i > 0)
            {
                line.drawWuLine(
                    anterior.getX(),
                    anterior.getY(),
                    ponto.getX(),
                    ponto.getY(),
                    Color(0, 0, 0)
                );

                anterior = ponto;
            }

            i++;
        }
    }

    // Permite que a Toolbar desenhe em toda a parte superior
    context->setViewport(0, 0, 640, 40);

    toolbar.render();
}

int main()
{
    // SDL
    SDL_Window* pWindow = nullptr;
    SDL_Renderer* pRenderer = nullptr;
    SDL_Surface* window_surface = nullptr;
    SDL_Event event;

    // Inicializa o SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        // Cria a janela
        pWindow = SDL_CreateWindow(
            "SDL_Classes",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            640,
            480,
            SDL_WINDOW_SHOWN
        );

        if (pWindow != nullptr)
        {
            pRenderer = SDL_CreateRenderer(
                pWindow,
                -1,
                0
            );

            window_surface = SDL_GetWindowSurface(pWindow);

            // Inicializa o contexto grafico da aplicacao
            Context* context = Context::getInstance();

            context->setRenderer(pRenderer);
            context->setWindowSurface(window_surface);
        }
    }
    else
    {
        return 1;
    }

    Canvas canvas(640, 440);
    Toolbar toolbar(640, 40);

    while (1)
    {
        // Limpa a tela
        canvas.clear();

        // Realiza o desenho
        display(canvas, toolbar);

        while (SDL_PollEvent(&event))
        {
            // Fecha a janela
            if (event.type == SDL_QUIT)
            {
                SDL_Quit();
                return 0;
            }

            // Primeiro deixa a Toolbar tratar o evento
            bool clickedUI = toolbar.handleEvent(event);

            // Se o evento nao pertence a Toolbar, trata o Canvas
            if (!clickedUI)
            {
                // Mouse pressionado
                if (event.type == SDL_MOUSEBUTTONDOWN &&
                    event.button.button == SDL_BUTTON_LEFT)
                {
                    Point clickPoint(
                        event.button.x,
                        event.button.y
                    );

                    // Criacao de uma figura que precise de dois pontos para ser criada
                    if (toolbar.getCurrentTool() == TOOL_LINE
                        || toolbar.getCurrentTool() == TOOL_RECTANGLE
                        || toolbar.getCurrentTool() == TOOL_CIRCLE)
                    {
                        drawing = true;

                        startPoint = clickPoint;
                        currentPoint = clickPoint;
                    }

                    // Curva utiliza quatro cliques para definir os quatro pontos
                    else if (toolbar.getCurrentTool() == TOOL_CURVE)
                    {
                        if (curvePointCount < 4)
                        {
                            curvePoints[curvePointCount] = clickPoint;
                            curvePointCount++;

                            // Cria a curva somente depois do quarto clique
                            if (curvePointCount == 4)
                            {
                                Curve* curve = new Curve(
                                    curvePoints,
                                    Color(0, 0, 0)
                                );

                                canvas.addShape(curve);

                                curvePointCount = 0;
                            }
                        }
                    }

                    // Adiciona um ponto ao poligono conforme clique do mouse
                    else if (toolbar.getCurrentTool() == TOOL_POLYGON)
                    {
                        polygonPoints.push_back(clickPoint);
                    }

                    // Executa o Flood Fill no ponto clicado
                    else if (toolbar.getCurrentTool() == TOOL_FLOOD_FILL)
                    {
                        canvas.addFloodFill(
                            Point(event.button.x, event.button.y),
                            Color(255, 0, 0)
                        );
                    }
                }

                // Mouse movimentando
                // Mouse movimentando
                if (event.type == SDL_MOUSEMOTION && drawing)
                {
                    currentPoint = Point(
                        event.motion.x,
                        event.motion.y
                    );

                    // Não permite desenhar fora da área do Canvas
                    if (currentPoint.getX() < 0)
                    {
                        currentPoint.setX(0);
                    }

                    if (currentPoint.getX() >= 640)
                    {
                        currentPoint.setX(639);
                    }

                    if (currentPoint.getY() < 40)
                    {
                        currentPoint.setY(40);
                    }

                    if (currentPoint.getY() >= 480)
                    {
                        currentPoint.setY(479);
                    }
                }

                // Mouse liberado
                if (event.type == SDL_MOUSEBUTTONUP &&
                    event.button.button == SDL_BUTTON_LEFT)
                {
                    // Impede que gere um ponto fora da area do canvas gerando crash
                    if (!canvas.isInside(event.button.x, event.button.y))
                    {
                        drawing = false;
                        continue;
                    }
                    // Cria a linha quando o usuario soltar o click do mouse
                    if (toolbar.getCurrentTool() == TOOL_LINE &&
                        drawing)
                    {
                        Point endPoint(
                            event.button.x,
                            event.button.y
                        );

                        Line* line = new Line(
                            startPoint,
                            endPoint,
                            Color(0, 0, 0)
                        );

                        canvas.addShape(line);

                        drawing = false;
                    }

                    // Cria o retangulo quando o usuario soltar o click do mouse
                    else if (toolbar.getCurrentTool() == TOOL_RECTANGLE &&
                             drawing)
                    {
                        Point endPoint(
                            event.button.x,
                            event.button.y
                        );

                        Rectangle* rectangle = new Rectangle(
                            startPoint,
                            endPoint,
                            Color(0, 0, 0)
                        );

                        canvas.addShape(rectangle);

                        drawing = false;
                    }

                    // Cria o circulo quando o usuario soltar o click do mouse
                    else if (toolbar.getCurrentTool() == TOOL_CIRCLE &&
                             drawing)
                    {
                        Point endPoint(
                            event.button.x,
                            event.button.y
                        );

                        double dx =
                            endPoint.getX() - startPoint.getX();

                        double dy =
                            endPoint.getY() - startPoint.getY();

                        double radius =
                            sqrt(dx * dx + dy * dy);

                        Circle* circle = new Circle(
                            startPoint,
                            radius,
                            Color(0, 0, 0)
                        );

                        canvas.addShape(circle);

                        drawing = false;
                    }

                    // A curva nao eh criada aqui pois precisa de quatro cliques
                }

                // Apertar botao direito do mouse completa o poligono
                if (event.type == SDL_MOUSEBUTTONDOWN &&
                    event.button.button == SDL_BUTTON_RIGHT)
                {
                    if (toolbar.getCurrentTool() == TOOL_POLYGON)
                    {
                        if (polygonPoints.size() >= 3)
                        {
                            Polygon* polygon = new Polygon(
                                polygonPoints,
                                Color(0, 0, 0)
                            );

                            canvas.addShape(polygon);

                            polygonPoints.clear();
                        }
                    }
                }
            }
        }

        // Atualiza a janela
        SDL_UpdateWindowSurface(pWindow);

        usleep(10000);
    }
}
