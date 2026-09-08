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
#include "ImageSaver.h"

// Controle da curva
Point curvePoints[4];
int curvePointCount = 0;

// Controle do poligono
list<Point> polygonPoints;

// Controle temporário do desenho
bool drawing = false;
Point startPoint;
Point currentPoint;

// Controle da figura selecionada
Canvas::HandleType activeHandle = Canvas::HANDLE_NONE;
bool transforming = false;
Point lastMousePosition;

// Desenha o conteúdo do Canvas e a interface
void display(Canvas& canvas, Toolbar& toolbar)
{
    Context* context = Context::getInstance();

    context->setViewport(0, 40, 640, 440);
    canvas.draw();

    // Preview da linha
    if (drawing && toolbar.getCurrentTool() == TOOL_LINE)
    {
        Line preview(startPoint, currentPoint, Color(0, 0, 0));
        preview.draw();
    }

    // Preview do retangulo
    if (drawing && toolbar.getCurrentTool() == TOOL_RECTANGLE)
    {
        Rectangle preview(startPoint, currentPoint, Color(0, 0, 0));
        preview.draw();
    }

    // Preview do circulo
    if (drawing && toolbar.getCurrentTool() == TOOL_CIRCLE)
    {
        double dx = currentPoint.getX() - startPoint.getX();
        double dy = currentPoint.getY() - startPoint.getY();
        double radius = sqrt(dx * dx + dy * dy);

        Circle preview(startPoint, radius, Color(0, 0, 0));
        preview.draw();
    }

    // Mostra os segmentos do poligono
    if (toolbar.getCurrentTool() == TOOL_POLYGON && polygonPoints.size() >= 2)
    {
        Line line;
        Point anterior = polygonPoints.front();
        int i = 0;

        for (Point ponto : polygonPoints)
        {
            if (i > 0)
            {
                line.drawWuLine(anterior.getX(), anterior.getY(), ponto.getX(), ponto.getY(), Color(0, 0, 0));
                anterior = ponto;
            }

            i++;
        }
    }

    context->setViewport(0, 0, 640, 40);
    toolbar.render();
}

int main()
{
    SDL_Window* pWindow = nullptr;
    SDL_Renderer* pRenderer = nullptr;
    SDL_Surface* window_surface = nullptr;
    SDL_Event event;

    // Inicializa o SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        return 1;

    // Cria a janela
    pWindow = SDL_CreateWindow("SDL_Classes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);

    if (pWindow == nullptr)
    {
        SDL_Quit();
        return 1;
    }

    pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
    window_surface = SDL_GetWindowSurface(pWindow);

    // Inicializa o contexto grafico
    Context* context = Context::getInstance();
    context->setRenderer(pRenderer);
    context->setWindowSurface(window_surface);

    Canvas canvas(640, 440);
    Toolbar toolbar(640, 40);
    ImageSaver imageSaver;

    while (1)
    {
        canvas.clear();
        display(canvas, toolbar);

        while (SDL_PollEvent(&event))
        {
            // Fecha a janela
            if (event.type == SDL_QUIT)
            {
                SDL_Quit();
                return 0;
            }

            // Primeiro a Toolbar trata o evento
            bool clickedUI = toolbar.handleEvent(event);

            if (toolbar.getCurrentTool() != TOOL_SELECT)
            {
                canvas.clearSelection();
                activeHandle = Canvas::HANDLE_NONE;
                transforming = false;
            }

            if (clickedUI)
                continue;

            // Mouse pressionado
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                Point clickPoint(event.button.x, event.button.y);

                // Ferramenta de selecao
                if (toolbar.getCurrentTool() == TOOL_SELECT)
                {
                    Canvas::HandleType handle = canvas.getHandleAt(event.button.x, event.button.y);

                    if (handle != Canvas::HANDLE_NONE)
                    {
                        activeHandle = handle;
                        transforming = true;
                        lastMousePosition = clickPoint;

                        // Inicia a escala
                        if (handle == Canvas::HANDLE_SCALE_TOP_LEFT ||
                            handle == Canvas::HANDLE_SCALE_TOP_RIGHT ||
                            handle == Canvas::HANDLE_SCALE_BOTTOM_LEFT ||
                            handle == Canvas::HANDLE_SCALE_BOTTOM_RIGHT)
                        {
                            canvas.startScale(handle, event.button.x, event.button.y);
                        }
                        else if (handle == Canvas::HANDLE_ROTATE)
                        {
                            canvas.startRotate(event.button.x, event.button.y);
                        }
                    }
                    else
                    {
                        canvas.selectShape(event.button.x, event.button.y);
                        transforming = false;
                        activeHandle = Canvas::HANDLE_NONE;
                    }
                }

                // Ferramentas que usam dois pontos
                else if (toolbar.getCurrentTool() == TOOL_LINE ||
                         toolbar.getCurrentTool() == TOOL_RECTANGLE ||
                         toolbar.getCurrentTool() == TOOL_CIRCLE)
                {
                    drawing = true;
                    startPoint = clickPoint;
                    currentPoint = clickPoint;
                }

                // Curva usa quatro pontos
                else if (toolbar.getCurrentTool() == TOOL_CURVE)
                {
                    if (curvePointCount < 4)
                    {
                        curvePoints[curvePointCount] = clickPoint;
                        curvePointCount++;

                        if (curvePointCount == 4)
                        {
                            Curve* curve = new Curve(curvePoints, Color(0, 0, 0));
                            canvas.addShape(curve);
                            curvePointCount = 0;
                        }
                    }
                }

                // Adiciona pontos ao poligono
                else if (toolbar.getCurrentTool() == TOOL_POLYGON)
                {
                    polygonPoints.push_back(clickPoint);
                }

                // Executa o Flood Fill
                else if (toolbar.getCurrentTool() == TOOL_FLOOD_FILL)
                {
                    canvas.addFloodFill(clickPoint, Color(173, 216, 230));
                }
                else if (toolbar.getCurrentTool() == TOOL_COLORBLACK)
                {
                    canvas.addFloodFill(clickPoint, Color(0, 0, 0));
                }
                else if (toolbar.getCurrentTool() == TOOL_COLORWHITE)
                {
                    canvas.addFloodFill(clickPoint, Color(255, 255, 255));
                }
                else if (toolbar.getCurrentTool() == TOOL_COLORBLUE)
                {
                    canvas.addFloodFill(clickPoint, Color(65,105,225));
                }
                else if (toolbar.getCurrentTool() == TOOL_COLORGREEN)
                {
                    canvas.addFloodFill(clickPoint, Color(172,225,175));
                }
            }

            // Mouse movimentando durante um desenho
            if (event.type == SDL_MOUSEMOTION && drawing)
            {
                currentPoint = Point(event.motion.x, event.motion.y);

                currentPoint.setX(std::max(0, std::min(639, currentPoint.getX())));
                currentPoint.setY(std::max(40, std::min(479, currentPoint.getY())));
            }

            // Mouse movimentando durante uma transformacao
            if (event.type == SDL_MOUSEMOTION && transforming)
            {
                Shape* selectedShape = canvas.getSelectedShape();

                if (selectedShape != nullptr)
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;

                    double dx = mouseX - lastMousePosition.getX();
                    double dy = mouseY - lastMousePosition.getY();

                    // Movimentacao
                    if (activeHandle == Canvas::HANDLE_TRANSLATE)
                    {
                        selectedShape->translate(dx, dy);
                        lastMousePosition = Point(mouseX, mouseY);
                    }
                    else if (activeHandle == Canvas::HANDLE_SCALE_TOP_LEFT ||
                             activeHandle == Canvas::HANDLE_SCALE_TOP_RIGHT ||
                             activeHandle == Canvas::HANDLE_SCALE_BOTTOM_LEFT ||
                             activeHandle == Canvas::HANDLE_SCALE_BOTTOM_RIGHT)
                    {
                        canvas.updateScale(activeHandle, mouseX, mouseY);
                    }
                    else if (activeHandle == Canvas::HANDLE_ROTATE)
                    {
                        canvas.updateRotate(mouseX, mouseY);
                    }
                }
            }

            // Mouse liberado
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
            {
                // Finaliza uma transformacao
                if (transforming)
                {
                    transforming = false;
                    activeHandle = Canvas::HANDLE_NONE;
                    continue;
                }

                // Evita criar figuras fora do Canvas
                if (!canvas.isInside(event.button.x, event.button.y))
                {
                    drawing = false;
                    continue;
                }

                // Cria a linha
                if (toolbar.getCurrentTool() == TOOL_LINE && drawing)
                {
                    Line* line = new Line(startPoint, Point(event.button.x, event.button.y), Color(0, 0, 0));
                    canvas.addShape(line);
                    drawing = false;
                }

                // Cria o retangulo
                else if (toolbar.getCurrentTool() == TOOL_RECTANGLE && drawing)
                {
                    Rectangle* rectangle = new Rectangle(startPoint, Point(event.button.x, event.button.y), Color(0, 0, 0));
                    canvas.addShape(rectangle);
                    drawing = false;
                }

                // Cria o circulo
                else if (toolbar.getCurrentTool() == TOOL_CIRCLE && drawing)
                {
                    Point endPoint(event.button.x, event.button.y);
                    double dx = endPoint.getX() - startPoint.getX();
                    double dy = endPoint.getY() - startPoint.getY();
                    double radius = sqrt(dx * dx + dy * dy);

                    Circle* circle = new Circle(startPoint, radius, Color(0, 0, 0));
                    canvas.addShape(circle);
                    drawing = false;
                }
            }

            // Botao direito finaliza o poligono
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
            {
                if (toolbar.getCurrentTool() == TOOL_POLYGON && polygonPoints.size() >= 3)
                {
                    Polygon* polygon = new Polygon(polygonPoints, Color(0, 0, 0));
                    canvas.addShape(polygon);
                    polygonPoints.clear();
                }
            }

            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_DELETE &&
                toolbar.getCurrentTool() == TOOL_SELECT)
            {
                canvas.deleteSelectedShape();

                activeHandle = Canvas::HANDLE_NONE;
                transforming = false;

                continue;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s && (SDL_GetModState() & KMOD_CTRL))
            {
                imageSaver.saveFile(window_surface, 0, 40, 640, 440);
                continue;
            }
        }

        SDL_UpdateWindowSurface(pWindow);
        usleep(10000);
    }
}
