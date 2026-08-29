#include "Toolbar.h"

Toolbar::Toolbar(int width, int height) : width(width), height(height) {
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

bool Toolbar::handleEvent(const SDL_Event& event) {
    //verifica clique botao esquerdo
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
        int mx = event.button.x;
        int my = event.button.y;

        if(my <= height){
            for(const auto& btn : buttons){
                if(mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h){
                    currentTool = btn.tool;
                    return true;
                }
            }
            return true;
        }
    }
    return false;
}

void Toolbar::render(){
    Color gray(200, 200, 200);
    Color darkGray(130, 130, 130);
    Color blue(100, 149, 237);

    Rectangle background(Point(0, 0), width, height, gray);
    background.draw();

    for (const auto& btn : buttons) {
        Color btnColor = (currentTool == btn.tool) ? blue : darkGray;
        Rectangle btnRect(Point(btn.x, btn.y), btn.w, btn.h, btnColor);
        btnRect.draw();
    }
}

ToolType Toolbar::getCurrentTool() const{
    return currentTool;
}

void Toolbar::setTool(ToolType tool){
    currentTool = tool;
}
