#include "Toolbar.h"
#include "Line.h"
#include "Circle.h"
#include "Curve.h"
#include "Polygon.h"
#include "Context.h"
#include "FloodFill.h"

Toolbar::Toolbar(int width, int height) : width(width), height(height)
{
    currentTool = TOOL_LINE;

    buttons.push_back({10,  5, 60, 30, TOOL_LINE});
    buttons.push_back({80,  5, 60, 30, TOOL_RECTANGLE});
    buttons.push_back({150, 5, 60, 30, TOOL_CIRCLE});
    buttons.push_back({220, 5, 60, 30, TOOL_CURVE});
    buttons.push_back({290, 5, 60, 30, TOOL_POLYGON});
    buttons.push_back({360, 5, 60, 30, TOOL_FLOOD_FILL});
    buttons.push_back({430, 5, 80, 30, TOOL_SELECT});
}

Toolbar::~Toolbar() {}

bool Toolbar::handleEvent(const SDL_Event& event)
{
    //verifica clique botao esquerdo
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mx = event.button.x;
        int my = event.button.y;

        if(my <= height)
        {
            for(const auto& btn : buttons)
            {
                if(mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h)
                {
                    currentTool = btn.tool;
                    return true;
                }
            }
            return true;
        }
    }
    return false;
}

void Toolbar::render()
{
    Color gray(200, 200, 200);
    Color darkGray(130, 130, 130);
    Color blue(100, 149, 237);
    Color black(0, 0, 0);

    // Preenche o fundo da Toolbar
    Line line;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            line.setPixel(x, y, gray);
        }
    }

    // Desenha os botões
    for (const auto& btn : buttons)
    {
        Color btnColor =
            (currentTool == btn.tool) ? blue : darkGray;

        Rectangle btnRect(
            Point(btn.x, btn.y),
            btn.w,
            btn.h,
            btnColor
        );

        btnRect.draw();

        int cx = btn.x + btn.w / 2;
        int cy = btn.y + btn.h / 2;

        switch (btn.tool)
        {
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

        case TOOL_CURVE:{
             Point pts[4] = {
             Point(btn.x + 10, cy + 8),
             Point(cx - 6, cy - 10),
             Point(cx + 6, cy + 10),
             Point(btn.x + btn.w - 10, cy + -8)
             };
             Curve curve(pts, black);
             curve.draw();
             break;
        }
        case TOOL_POLYGON:{
             std::list<Point> pts ={
                 Point(cx, cy - 10),
                 Point(cx + 9, cy-5),
                 Point(cx + 9, cy + 5),
                 Point(cx, cy + 10),
                 Point(cx - 9, cy+ 5),
                 Point(cx - 9, cy - 5)
             };
             Polygon polygon(pts, black);
             polygon.draw();
             break;

        }
        case TOOL_SELECT:{
            Line seta(Point(cx - 10, cy),Point(cx + 4, cy),black);
            seta.draw();

            std::list<Point> pts ={
                Point(cx + 10,cy),
                Point(cx + 4, cy - 5),
                Point(cx + 4, cy + 5)
            };

            Polygon ponta(pts, black);
            ponta.draw();
            break;
        }
        case TOOL_FLOOD_FILL:{
              //gota
              Point gotaTopo(cx, cy - 8);
              Point lineEsquerda(cx - 6, cy + 4);
              Point lineDireita(cx + 6, cy + 4);

               //retas
              Line line1(gotaTopo, lineEsquerda, black);
              line1.draw();

              Line line2(gotaTopo, lineDireita, black);
              line2.draw();
               //curva
              Point curvePts[4] ={
                   lineEsquerda,
                   Point(cx -6, cy + 14),
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

ToolType Toolbar::getCurrentTool() const
{
    return currentTool;
}

void Toolbar::setTool(ToolType tool)
{
    currentTool = tool;
}
