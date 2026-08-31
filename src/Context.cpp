#include "Context.h"

Context* Context::_instance = nullptr;

Context::Context()
{
    // Inicializa o viewport com o tamanho da janela
    _viewportX = 0;
    _viewportY = 0;
    _viewportWidth = 640;
    _viewportHeight = 480;
}

Context::~Context()
{
}

Context* Context::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new Context();
    }

    return _instance;
}

void Context::setWindowSurface(SDL_Surface* window_surface)
{
    _window_surface = window_surface;
}

void Context::setRenderer(SDL_Renderer* pRenderer)
{
    _pRenderer = pRenderer;
}

SDL_Surface* Context::getWindowSurface()
{
    return _window_surface;
}

SDL_Renderer* Context::getRenderer()
{
    return _pRenderer;
}

// Define a área onde as formas podem ser desenhadas
void Context::setViewport(int x, int y, int width, int height)
{
    _viewportX = x;
    _viewportY = y;
    _viewportWidth = width;
    _viewportHeight = height;
}

// Verifica se o pixel está dentro do viewport
bool Context::isInsideViewport(int x, int y)
{
    return x >= _viewportX &&
           x < _viewportX + _viewportWidth &&
           y >= _viewportY &&
           y < _viewportY + _viewportHeight;
}
