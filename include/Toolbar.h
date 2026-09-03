#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <vector>
#include <SDL2/SDL.h>
#include "Rectangle.h"
#include "Color.h"

enum ToolType {
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_CURVE,
    TOOL_POLYGON,
    TOOL_FLOOD_FILL,
    TOOL_SELECT,
    TOOL_COLORBLUE,
    TOOL_COLORWHITE,
    TOOL_COLORGREEN,
    TOOL_COLORBLACK
};

struct ButtonArea{
    int x, y, w, h;
    ToolType tool;
};

class Toolbar{
private:
    std::vector<ButtonArea> buttons;
    ToolType currentTool;
    int width;
    int height;

public:
    Toolbar(int width = 640, int height = 40);
    ~Toolbar();
    bool handleEvent(const SDL_Event& event);
    void render();

    ToolType getCurrentTool() const;
    void setTool(ToolType tool);
};

#endif
