#include "Context.h"

Context * Context::_instance = nullptr;

Context::Context()
{
    //ctor
}

Context::~Context()
{
    //dtor
}

Context * Context::getInstance()
{
    if(_instance == 0) {
        _instance = new Context();
    }
    return _instance;
}


void Context::setWindowSurface(SDL_Surface * window_surface)
{
    _window_surface = window_surface;
}

void Context::setRenderer(SDL_Renderer * pRenderer)
{
    _pRenderer = pRenderer;
}

SDL_Surface * Context::getWindowSurface()
{
    return _window_surface;
}

SDL_Renderer* Context::getRenderer()
{
    return _pRenderer;
}

