#ifndef CONTEXT_H
#define CONTEXT_H

#include <SDL2/SDL.h>

class Context
{
private:
    static Context* _instance;

    SDL_Surface* _window_surface;
    SDL_Renderer* _pRenderer;

    int _viewportX;
    int _viewportY;
    int _viewportWidth;
    int _viewportHeight;

    Context();

public:
    ~Context();

    static Context* getInstance();

    void setWindowSurface(SDL_Surface* window_surface);
    void setRenderer(SDL_Renderer* pRenderer);

    SDL_Surface* getWindowSurface();
    SDL_Renderer* getRenderer();

    void setViewport(int x, int y, int width, int height);
    bool isInsideViewport(int x, int y);
};

#endif // CONTEXT_H
