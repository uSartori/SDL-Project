#include "Toolbar.h"
#include "Line.h"
#include "Circle.h"
#include "Curve.h"
#include "Polygon.h"
#include "Context.h"
#include "FloodFill.h"

// Construtor da Toolbar
// Define o tamanho da interface e cria os botões disponíveis
Toolbar::Toolbar(int width, int height) : width(width), height(height)
{
    // Define a ferramenta de linha como ferramenta inicial
    currentTool = TOOL_LINE;

    // Cria os botões informando posição, tamanho e ferramenta associada
    buttons.push_back({10,  5, 60, 30, TOOL_LINE});
    buttons.push_back({80,  5, 60, 30, TOOL_RECTANGLE});
    buttons.push_back({150, 5, 60, 30, TOOL_CIRCLE});
    buttons.push_back({220, 5, 60, 30, TOOL_CURVE});
    buttons.push_back({290, 5, 60, 30, TOOL_POLYGON});
    buttons.push_back({360, 5, 60, 30, TOOL_SELECT});

    // Cria os botões utilizados para selecionar a cor
    buttons.push_back({505, 5, 20, 30, TOOL_COLORBLUE});
    buttons.push_back({540, 5, 20, 30, TOOL_COLORWHITE});
    buttons.push_back({575, 5, 20, 30, TOOL_COLORBLACK});
    buttons.push_back({610, 5, 20, 30, TOOL_COLORGREEN});
}

// Destrutor da Toolbar
Toolbar::~Toolbar() {}

// Processa os eventos relacionados à Toolbar
bool Toolbar::handleEvent(const SDL_Event& event)
{
    // Verifica se o usuário pressionou o botão esquerdo do mouse
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mx = event.button.x;
        int my = event.button.y;

        // Verifica se o clique ocorreu na região ocupada pela Toolbar
        if(my <= height)
        {
            // Percorre todos os botões para verificar qual foi clicado
            for(const auto& btn : buttons)
            {
                // Verifica se a posição do mouse está dentro dos limites do botão
                if(mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h)
                {
                    // Define a ferramenta clicada como a ferramenta atual
                    currentTool = btn.tool;

                    // Informa que o evento já foi tratado pela Toolbar
                    return true;
                }
            }

            // Impede que cliques na área da Toolbar sejam tratados pelo Canvas
            return true;
        }
    }

    // Informa que o evento não pertence à Toolbar
    return false;
}

// Desenha a Toolbar na tela
void Toolbar::render()
{
    // Define as cores utilizadas na Toolbar e nos ícones
    Color gray(200, 200, 200);
    Color darkGray(130, 130, 130);
    Color select(100, 149, 237);
    Color black(0, 0, 0);
    Color white(255,255,255);
    Color green(172,225,175);
    Color blue(65,105,225);

    // Utiliza uma linha para alterar diretamente os pixels da Toolbar
    Line line;

    // Objeto utilizado anteriormente para preenchimento
    FloodFill floodFill;

    // Preenche o fundo da Toolbar com a cor cinza
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            line.setPixel(x, y, gray);
        }
    }

    // Cria uma pequena faixa inferior mais escura
    for (int x = 0; x < width; x++)
    {
        for (int y = 39; y < height; y++)
        {
            line.setPixel(x, y, darkGray);
        }
    }

    // Percorre todos os botões para desenhar cada um deles
    for (const auto& btn : buttons)
    {
        // Destaca o botão da ferramenta atualmente selecionada
        Color btnColor =
            (currentTool == btn.tool) ? select : darkGray;

        // Cria o retângulo que representa o botão
        Rectangle btnRect(
            Point(btn.x, btn.y),
            btn.w,
            btn.h,
            btnColor
        );

        btnRect.draw();

        // Calcula o centro do botão para posicionar os ícones
        int cx = btn.x + btn.w / 2;
        int cy = btn.y + btn.h / 2;

        // Define qual ícone será desenhado para cada ferramenta
        switch (btn.tool)
        {
        // Ícone da ferramenta de linha
        case TOOL_LINE:
        {
            Line line(
                Point(btn.x + 12, btn.y + btn.h - 8),
                Point(btn.x + btn.w - 12, btn.y + 8),
                black
            );

            line.draw();
            break;
        }

        // Ícone da ferramenta de retângulo
        case TOOL_RECTANGLE:
        {
            Rectangle iconRect(
                Point(btn.x + 15, btn.y + 8),
                btn.w - 30,
                btn.h - 16,
                black
            );

            iconRect.draw();
            break;
        }

        // Ícone da ferramenta de círculo
        case TOOL_CIRCLE:
        {
            Circle circle(
                Point(cx, cy),
                12,
                black
            );

            circle.draw();
            break;
        }

        // Ícone da ferramenta de curva
        // Utiliza quatro pontos para criar uma pequena curva de Bezier
        case TOOL_CURVE:
        {
            Point pts[4] = {
                Point(btn.x + 10, cy + 8),
                Point(cx - 6, cy - 10),
                Point(cx + 6, cy + 10),
                Point(btn.x + btn.w - 10, cy - 8)
            };

            Curve curve(pts, black);
            curve.draw();
            break;
        }

        // Ícone da ferramenta de polígono
        // Utiliza vários pontos para formar um hexágono como representação
        case TOOL_POLYGON:
        {
            std::list<Point> pts = {
                Point(cx, cy - 10),
                Point(cx + 9, cy - 5),
                Point(cx + 9, cy + 5),
                Point(cx, cy + 10),
                Point(cx - 9, cy + 5),
                Point(cx - 9, cy - 5)
            };

            Polygon polygon(pts, black);
            polygon.draw();
            break;
        }

        // Ícone da ferramenta de seleção
        // É formado por uma linha e uma ponta de seta
        case TOOL_SELECT:
        {
            Line seta(
                Point(cx - 10, cy),
                Point(cx + 4, cy),
                black
            );

            seta.draw();

            std::list<Point> pts = {
                Point(cx + 10, cy),
                Point(cx + 4, cy - 5),
                Point(cx + 4, cy + 5)
            };

            Polygon ponta(pts, black);
            ponta.draw();
            break;
        }

        // Botão da cor azul
        case TOOL_COLORBLUE:
        {
            // Preenche a área do botão com azul
            for (int x = 506; x < 525; x++)
            {
                for (int y = 6; y < 35; y++)
                {
                    line.setPixel(x, y, blue);
                }
            }

            // Cria o desenho de uma gota para representar a seleção de cor
            Point gotaTopo(cx, cy - 8);
            Point lineEsquerda(cx - 6, cy + 4);
            Point lineDireita(cx + 6, cy + 4);

            // Desenha as laterais da gota
            Line line1(gotaTopo, lineEsquerda, white);
            line1.draw();

            Line line2(gotaTopo, lineDireita, white);
            line2.draw();

            // Desenha a parte arredondada da gota com uma curva de Bezier
            Point curvePts[4] = {
                lineEsquerda,
                Point(cx - 6, cy + 14),
                Point(cx + 9, cy + 12),
                lineDireita
            };

            Curve gotaCurve(curvePts, white);
            gotaCurve.draw();

            break;
        }

        // Botão da cor branca
        case TOOL_COLORWHITE:
        {
            // Preenche a área do botão com branco
            for (int x = 541; x < 560; x++)
            {
                for (int y = 6; y < 35; y++)
                {
                    line.setPixel(x, y, white);
                }
            }

            // Cria o desenho da gota
            Point gotaTopo(cx, cy - 8);
            Point lineEsquerda(cx - 6, cy + 4);
            Point lineDireita(cx + 6, cy + 4);

            // Usa preto no contorno para deixar a gota visível sobre o branco
            Line line1(gotaTopo, lineEsquerda, black);
            line1.draw();

            Line line2(gotaTopo, lineDireita, black);
            line2.draw();

            Point curvePts[4] = {
                lineEsquerda,
                Point(cx - 6, cy + 14),
                Point(cx + 9, cy + 12),
                lineDireita
            };

            Curve gotaCurve(curvePts, black);
            gotaCurve.draw();

            break;
        }
        case TOOL_FLOOD_FILL:
        {
                break;
        }

        // Botão da cor preta
        case TOOL_COLORBLACK:
        {
            // Preenche a área do botão com preto
            for (int x = 576; x < 595; x++)
            {
                for (int y = 6; y < 35; y++)
                {
                    line.setPixel(x, y, black);
                }
            }

            // Cria o desenho da gota
            Point gotaTopo(cx, cy - 8);
            Point lineEsquerda(cx - 6, cy + 4);
            Point lineDireita(cx + 6, cy + 4);

            // Usa branco no contorno para destacar a gota sobre o fundo preto
            Line line1(gotaTopo, lineEsquerda, white);
            line1.draw();

            Line line2(gotaTopo, lineDireita, white);
            line2.draw();

            Point curvePts[4] = {
                lineEsquerda,
                Point(cx - 6, cy + 14),
                Point(cx + 9, cy + 12),
                lineDireita
            };

            Curve gotaCurve(curvePts, white);
            gotaCurve.draw();

            break;
        }

        // Botão da cor verde
        case TOOL_COLORGREEN:
        {
            // Preenche a área do botão com verde
            for (int x = 611; x < 630; x++)
            {
                for (int y = 6; y < 35; y++)
                {
                    line.setPixel(x, y, green);
                }
            }

            // Cria o desenho da gota
            Point gotaTopo(cx, cy - 8);
            Point lineEsquerda(cx - 6, cy + 4);
            Point lineDireita(cx + 6, cy + 4);

            // Usa preto no contorno para destacar a gota sobre o fundo verde
            Line line1(gotaTopo, lineEsquerda, black);
            line1.draw();

            Line line2(gotaTopo, lineDireita, black);
            line2.draw();

            Point curvePts[4] = {
                lineEsquerda,
                Point(cx - 6, cy + 14),
                Point(cx + 9, cy + 12),
                lineDireita
            };

            Curve gotaCurve(curvePts, black);
            gotaCurve.draw();

            break;
        }
        }
    }
}

// Retorna a ferramenta atualmente selecionada
// É utilizada pelo main para descobrir qual operação deve ser executada
ToolType Toolbar::getCurrentTool() const
{
    return currentTool;
}

// Altera a ferramenta atualmente selecionada
// Permite que outra parte do programa selecione uma ferramenta diretamente
void Toolbar::setTool(ToolType tool)
{
    currentTool = tool;
}
