#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include <cmath>

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

// SDL
SDL_Window* pWindow = nullptr;
SDL_Renderer* pRenderer = nullptr;
SDL_Surface* window_surface = nullptr;

int offset = 10;
Point curvePoints[4];
int curvePointCount = 0;

// Controle temporário do desenho da linha
bool drawing = false;
Point startPoint;
Point currentPoint;

// Desenha o conteúdo do Canvas e a interface
void display(Canvas& canvas, Toolbar& toolbar)
{
    toolbar.render();
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
}

void clear()
{
    Line line;

    SDL_Surface* window_surface =
        Context::getInstance()->getWindowSurface();

    for (int x = 0; x < window_surface->w; x++)
    {
        for (int y = 0; y < window_surface->h; y++)
        {
            line.setPixel(x, y, 255, 255, 255);
        }
    }
}

int main(int argc, char* args[])
{
    SDL_Event event;

    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
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

        // Desenha Canvas, pré-visualização e Toolbar
        display(canvas, toolbar);

        while (SDL_PollEvent(&event))
        {
            // Fecha a janela
            if (event.type == SDL_QUIT)
            {
                return 0;
            }

            // Primeiro deixa a Toolbar tratar o evento
            bool clickedUI = toolbar.handleEvent(event);

            // Se o evento pertence à Toolbar, ignora o restante
            if (clickedUI)
            {
                continue;
            }
            // Se o evento não pertence à Toolbar, trata o Canvas
            if (!clickedUI)
            {
                // Mouse pressionado
                if (event.type == SDL_MOUSEBUTTONDOWN &&
                    event.button.button == SDL_BUTTON_LEFT)
                {
                    // Criação de uma figura que precise de dois pontos para ser criada
                    if (toolbar.getCurrentTool() == TOOL_LINE
                        || toolbar.getCurrentTool() == TOOL_RECTANGLE
                        || toolbar.getCurrentTool() == TOOL_CIRCLE)
                    {
                        drawing = true;

                        startPoint = Point(
                            event.button.x,
                            event.button.y
                        );

                        currentPoint = startPoint;
                    }
                    // Curva utiliza quatro cliques para definir os quatro pontos
                    if (toolbar.getCurrentTool() == TOOL_CURVE)
                    {
                        if (curvePointCount < 4)
                        {
                            curvePoints[curvePointCount] = Point(
                                event.button.x,
                                event.button.y
                            );

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
                }
                }

                // Mouse movimentando
                if (event.type == SDL_MOUSEMOTION && drawing)
                {
                    currentPoint = Point(
                        event.motion.x,
                        event.motion.y
                    );
                }

                // Mouse liberado
                if (event.type == SDL_MOUSEBUTTONUP &&
                    event.button.button == SDL_BUTTON_LEFT)
                {
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
                    if (toolbar.getCurrentTool() == TOOL_RECTANGLE && drawing)
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
                    if (toolbar.getCurrentTool() == TOOL_CIRCLE && drawing)
                    {
                        Point endPoint(
                            event.button.x,
                            event.button.y
                        );

                        double dx = endPoint.getX() - startPoint.getX();
                        double dy = endPoint.getY() - startPoint.getY();

                        double radius = sqrt(dx * dx + dy * dy);

                        Circle* circle = new Circle(
                            startPoint,
                            radius,
                            Color(0, 0, 0)
                        );

                        canvas.addShape(circle);

                        drawing = false;
                    }
                    // A curva nao eh criada aqui pois precisa de quatro cliques e nao soltar o click

                }
            }


        // Atualiza a janela
        SDL_UpdateWindowSurface(pWindow);

        usleep(10000);
    }

    SDL_Quit();

    return 0;
}
