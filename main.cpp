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

// Armazena os quatro pontos necessários para criar uma curva de Bezier
Point curvePoints[4];
int curvePointCount = 0;

// Armazena temporariamente os pontos do polígono enquanto ele está sendo desenhado
list<Point> polygonPoints;

// Controla o desenho temporário das figuras que utilizam dois pontos
bool drawing = false;
Point startPoint;
Point currentPoint;

// Controla a transformação da figura selecionada
Canvas::HandleType activeHandle = Canvas::HANDLE_NONE;
bool transforming = false;
Point lastMousePosition;

// Desenha o conteúdo do Canvas e a interface
void display(Canvas& canvas, Toolbar& toolbar)
{
    Context* context = Context::getInstance();

    // Define a área onde o Canvas será desenhado, deixando os primeiros 40 pixels para a Toolbar
    context->setViewport(0, 40, 640, 440);
    canvas.draw();

    // Mostra uma prévia da linha enquanto o usuário movimenta o mouse
    // A figura só é adicionada ao Canvas quando o botão do mouse é solto
    if (drawing && toolbar.getCurrentTool() == TOOL_LINE)
    {
        Line preview(startPoint, currentPoint, Color(0, 0, 0));
        preview.draw();
    }

    // Mostra uma prévia do retângulo enquanto o usuário movimenta o mouse
    if (drawing && toolbar.getCurrentTool() == TOOL_RECTANGLE)
    {
        Rectangle preview(startPoint, currentPoint, Color(0, 0, 0));
        preview.draw();
    }

    // Mostra uma prévia do círculo calculando o raio a partir da distância entre os dois pontos
    if (drawing && toolbar.getCurrentTool() == TOOL_CIRCLE)
    {
        double dx = currentPoint.getX() - startPoint.getX();
        double dy = currentPoint.getY() - startPoint.getY();
        double radius = sqrt(dx * dx + dy * dy);

        Circle preview(startPoint, radius, Color(0, 0, 0));
        preview.draw();
    }

    // Mostra temporariamente os segmentos do polígono conforme os pontos são adicionados
    // Isso permite visualizar o polígono antes de finalizá-lo com o botão direito
    if (toolbar.getCurrentTool() == TOOL_POLYGON && polygonPoints.size() >= 2)
    {
        Line line;
        Point anterior = polygonPoints.front();
        int i = 0;

        for (Point ponto : polygonPoints)
        {
            if (i > 0)
            {
                // Usa a linha de Wu para desenhar os segmentos com antialiasing
                line.drawWuLine(anterior.getX(), anterior.getY(), ponto.getX(), ponto.getY(), Color(0, 0, 0));
                anterior = ponto;
            }

            i++;
        }
    }

    // Volta o viewport para a área da Toolbar
    context->setViewport(0, 0, 640, 40);
    toolbar.render();
}

int main()
{
    SDL_Window* pWindow = nullptr;
    SDL_Renderer* pRenderer = nullptr;
    SDL_Surface* window_surface = nullptr;
    SDL_Event event;

    // Inicializa todos os subsistemas necessários do SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        return 1;

    // Cria a janela principal do programa com tamanho de 640x480 pixels
    pWindow = SDL_CreateWindow("SDL_Classes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);

    // Verifica se a janela foi criada corretamente
    // Caso contrário, encerra o SDL para liberar os recursos já inicializados
    if (pWindow == nullptr)
    {
        SDL_Quit();
        return 1;
    }

    // Cria o Renderer utilizado pelo SDL
    pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

    // Obtém a Surface da janela, utilizada para acessar diretamente seus pixels
    window_surface = SDL_GetWindowSurface(pWindow);

    // Obtém a instância única do Context e configura os recursos gráficos utilizados pelo programa
    Context* context = Context::getInstance();
    context->setRenderer(pRenderer);
    context->setWindowSurface(window_surface);

    // Cria o Canvas e a Toolbar com os tamanhos definidos para a aplicação
    Canvas canvas(640, 440);
    Toolbar toolbar(640, 40);
    ImageSaver imageSaver;

    // Exibe instruções básicas no terminal para facilitar o uso do programa
    printf("======== Observacoes ========\n");
    printf("Botao direito conclui o poligono apos tres ou mais pontos\n");
    printf("Delete apaga a figura selecionada\n");
    printf("Ctrl + S salva a imagem do canvas\n");

    // Loop principal do programa
    // Continua executando até que o usuário feche a janela
    while (1)
    {
        // Limpa o Canvas antes de redesenhar todos os elementos
        // Isso garante que alterações de posição, escala e rotação sejam atualizadas na tela
        canvas.clear();

        // Redesenha o Canvas, as figuras, as prévias e a Toolbar
        display(canvas, toolbar);

        // Processa todos os eventos que ocorreram desde a última iteração
        while (SDL_PollEvent(&event))
        {
            // Fecha a aplicação quando o usuário fecha a janela
            if (event.type == SDL_QUIT)
            {
                SDL_Quit();
                return 0;
            }

            // A Toolbar recebe o evento primeiro para verificar se algum botão foi clicado
            bool clickedUI = toolbar.handleEvent(event);

            // Quando uma ferramenta diferente da seleção é escolhida,
            // qualquer seleção ou transformação em andamento deve ser cancelada
            if (toolbar.getCurrentTool() != TOOL_SELECT)
            {
                canvas.clearSelection();
                activeHandle = Canvas::HANDLE_NONE;
                transforming = false;
            }

            // Se o evento foi tratado pela Toolbar, ele não precisa ser processado pelo Canvas
            if (clickedUI)
                continue;

            // Verifica o pressionamento do botão esquerdo do mouse
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                Point clickPoint(event.button.x, event.button.y);

                // Ferramenta de seleção
                if (toolbar.getCurrentTool() == TOOL_SELECT)
                {
                    // Verifica se o clique ocorreu sobre algum dos controles da figura selecionada
                    // Os controles permitem movimentar, escalar ou rotacionar a figura
                    Canvas::HandleType handle = canvas.getHandleAt(event.button.x, event.button.y);

                    if (handle != Canvas::HANDLE_NONE)
                    {
                        activeHandle = handle;
                        transforming = true;
                        lastMousePosition = clickPoint;

                        // Inicia a escala quando um dos quatro cantos da caixa de seleção é pressionado
                        if (handle == Canvas::HANDLE_SCALE_TOP_LEFT ||
                            handle == Canvas::HANDLE_SCALE_TOP_RIGHT ||
                            handle == Canvas::HANDLE_SCALE_BOTTOM_LEFT ||
                            handle == Canvas::HANDLE_SCALE_BOTTOM_RIGHT)
                        {
                            canvas.startScale(handle, event.button.x, event.button.y);
                        }

                        // Inicia a rotação quando o controle de rotação é pressionado
                        else if (handle == Canvas::HANDLE_ROTATE)
                        {
                            canvas.startRotate(event.button.x, event.button.y);
                        }
                    }
                    else
                    {
                        // Se nenhum controle foi pressionado, tenta selecionar uma figura
                        canvas.selectShape(event.button.x, event.button.y);

                        // Um clique normal não inicia uma transformação
                        transforming = false;
                        activeHandle = Canvas::HANDLE_NONE;
                    }
                }

                // Ferramentas que precisam de um ponto inicial e um ponto final
                else if (toolbar.getCurrentTool() == TOOL_LINE ||
                         toolbar.getCurrentTool() == TOOL_RECTANGLE ||
                         toolbar.getCurrentTool() == TOOL_CIRCLE)
                {
                    // Inicia o desenho e salva a posição inicial
                    drawing = true;
                    startPoint = clickPoint;
                    currentPoint = clickPoint;
                }

                // A curva de Bezier é criada utilizando quatro pontos de controle
                else if (toolbar.getCurrentTool() == TOOL_CURVE)
                {
                    if (curvePointCount < 4)
                    {
                        // Armazena o ponto clicado e incrementa a quantidade de pontos
                        curvePoints[curvePointCount] = clickPoint;
                        curvePointCount++;

                        // Quando os quatro pontos são informados, a curva pode ser criada
                        if (curvePointCount == 4)
                        {
                            Curve* curve = new Curve(curvePoints, Color(0, 0, 0));
                            canvas.addShape(curve);

                            // Reinicia a contagem para permitir criar outra curva
                            curvePointCount = 0;
                        }
                    }
                }

                // Adiciona os pontos clicados à lista temporária do polígono
                else if (toolbar.getCurrentTool() == TOOL_POLYGON)
                {
                    polygonPoints.push_back(clickPoint);
                }

                // Executa o Flood Fill utilizando a cor azul-clara
                else if (toolbar.getCurrentTool() == TOOL_FLOOD_FILL)
                {
                    canvas.addFloodFill(clickPoint, Color(173, 216, 230));
                }

                // Executa o Flood Fill utilizando a cor preta
                else if (toolbar.getCurrentTool() == TOOL_COLORBLACK)
                {
                    canvas.addFloodFill(clickPoint, Color(0, 0, 0));
                }

                // Executa o Flood Fill utilizando a cor branca
                else if (toolbar.getCurrentTool() == TOOL_COLORWHITE)
                {
                    canvas.addFloodFill(clickPoint, Color(255, 255, 255));
                }

                // Executa o Flood Fill utilizando a cor azul
                else if (toolbar.getCurrentTool() == TOOL_COLORBLUE)
                {
                    canvas.addFloodFill(clickPoint, Color(65, 105, 225));
                }

                // Executa o Flood Fill utilizando a cor verde
                else if (toolbar.getCurrentTool() == TOOL_COLORGREEN)
                {
                    canvas.addFloodFill(clickPoint, Color(172, 225, 175));
                }
            }

            // Atualiza a posição final durante o desenho de uma figura
            // Essa posição é utilizada para mostrar a prévia em tempo real
            if (event.type == SDL_MOUSEMOTION && drawing)
            {
                currentPoint = Point(event.motion.x, event.motion.y);

                // Mantém o mouse dentro da área válida do Canvas
                // O limite superior é 40 porque essa região pertence à Toolbar
                currentPoint.setX(std::max(0, std::min(639, currentPoint.getX())));
                currentPoint.setY(std::max(40, std::min(479, currentPoint.getY())));
            }

            // Atualiza a transformação enquanto o mouse é movimentado
            if (event.type == SDL_MOUSEMOTION && transforming)
            {
                Shape* selectedShape = canvas.getSelectedShape();

                // Só realiza a transformação se existir uma figura selecionada
                if (selectedShape != nullptr)
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;

                    // Calcula quanto o mouse se deslocou desde a última posição registrada
                    // Esses valores são utilizados para realizar a movimentação da figura
                    double dx = mouseX - lastMousePosition.getX();
                    double dy = mouseY - lastMousePosition.getY();

                    // Movimentação da figura
                    if (activeHandle == Canvas::HANDLE_TRANSLATE)
                    {
                        selectedShape->translate(dx, dy);
                        lastMousePosition = Point(mouseX, mouseY);
                    }

                    // Escala da figura utilizando um dos quatro cantos da caixa de seleção
                    else if (activeHandle == Canvas::HANDLE_SCALE_TOP_LEFT ||
                             activeHandle == Canvas::HANDLE_SCALE_TOP_RIGHT ||
                             activeHandle == Canvas::HANDLE_SCALE_BOTTOM_LEFT ||
                             activeHandle == Canvas::HANDLE_SCALE_BOTTOM_RIGHT)
                    {
                        canvas.updateScale(activeHandle, mouseX, mouseY);
                    }

                    // Atualiza a rotação da figura de acordo com a posição atual do mouse
                    else if (activeHandle == Canvas::HANDLE_ROTATE)
                    {
                        canvas.updateRotate(mouseX, mouseY);
                    }
                }
            }

            // Verifica quando o botão esquerdo do mouse é liberado
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
            {
                // Finaliza uma transformação que estava sendo realizada
                if (transforming)
                {
                    transforming = false;
                    activeHandle = Canvas::HANDLE_NONE;
                    continue;
                }

                // Impede que uma figura seja criada caso o mouse seja liberado fora do Canvas
                if (!canvas.isInside(event.button.x, event.button.y))
                {
                    drawing = false;
                    continue;
                }

                // Cria a linha utilizando o ponto inicial e a posição final do mouse
                if (toolbar.getCurrentTool() == TOOL_LINE && drawing)
                {
                    Line* line = new Line(startPoint, Point(event.button.x, event.button.y), Color(0, 0, 0));
                    canvas.addShape(line);
                    drawing = false;
                }

                // Cria o retângulo utilizando os dois pontos definidos pelo usuário
                else if (toolbar.getCurrentTool() == TOOL_RECTANGLE && drawing)
                {
                    Rectangle* rectangle = new Rectangle(startPoint, Point(event.button.x, event.button.y), Color(0, 0, 0));
                    canvas.addShape(rectangle);
                    drawing = false;
                }

                // Cria o círculo calculando o raio pela distância entre o ponto inicial e o final
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

            // O botão direito é utilizado para finalizar o polígono
            // São exigidos pelo menos três pontos para formar uma figura válida
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
            {
                if (toolbar.getCurrentTool() == TOOL_POLYGON && polygonPoints.size() >= 3)
                {
                    Polygon* polygon = new Polygon(polygonPoints, Color(0, 0, 0));
                    canvas.addShape(polygon);

                    // Limpa os pontos temporários para permitir a criação de um novo polígono
                    polygonPoints.clear();
                }
            }

            // Verifica o pressionamento da tecla Delete durante a ferramenta de seleção
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_DELETE &&
                toolbar.getCurrentTool() == TOOL_SELECT)
            {
                // Remove a figura atualmente selecionada do Canvas
                canvas.deleteSelectedShape();

                // Limpa o estado da seleção para evitar que os controles
                // continuem associados à figura que acabou de ser removida
                activeHandle = Canvas::HANDLE_NONE;
                transforming = false;

                continue;
            }

            // Verifica o atalho Ctrl + S para salvar somente a área do Canvas
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s && (SDL_GetModState() & KMOD_CTRL))
            {
                // Salva a região do Canvas, ignorando a Toolbar localizada nos primeiros 40 pixels
                imageSaver.saveFile(window_surface, 0, 40, 640, 440);
                continue;
            }
        }

        // Atualiza a Surface da janela com as alterações realizadas durante o frame
        SDL_UpdateWindowSurface(pWindow);

        // Pequena pausa para evitar que o loop rode continuamente consumindo todo o processador
        usleep(10000);
    }
}
