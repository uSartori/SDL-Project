#ifndef CONTEXT_H
#define CONTEXT_H
#include<SDL2/SDL.h>


class Context
{
    public:
        static Context * getInstance();
        void setWindowSurface(SDL_Surface * window_surface);
        void setRenderer(SDL_Renderer * pRenderer);
        SDL_Surface * getWindowSurface();
        SDL_Renderer * getRenderer();
    protected:
        Context();
    private:
        virtual ~Context();
        static Context * _instance;
        SDL_Surface * _window_surface;
        SDL_Renderer * _pRenderer;
};

#endif // CONTEXT_H
