#include "FloodFill.h"
#include "Context.h"

#include <stack>

// Construtor
FloodFill::FloodFill()
{
}

// Destrutor
FloodFill::~FloodFill()
{
}

// Retorna a cor de um pixel
Uint32 FloodFill::getPixel(int x, int y)
{
    SDL_Surface* surface = Context::getInstance()->getWindowSurface();

    if (surface == nullptr || x < 0 || x >= surface->w || y < 0 || y >= surface->h)
        return 0;

    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    if (bpp == 4)
        return *(Uint32*)p;

    if (bpp == 3)
    {
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];

        return p[0] | p[1] << 8 | p[2] << 16;
    }

    if (bpp == 2)
        return *(Uint16*)p;

    if (bpp == 1)
        return *p;

    return 0;
}

// Altera a cor de um pixel
void FloodFill::setPixel(int x, int y, Uint32 color)
{
    SDL_Surface* surface = Context::getInstance()->getWindowSurface();

    if (surface == nullptr || x < 0 || x >= surface->w || y < 40 || y >= surface->h)
        return;

    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    if (bpp == 4)
        *(Uint32*)p = color;
    else if (bpp == 3)
    {
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (color >> 16) & 0xFF;
            p[1] = (color >> 8) & 0xFF;
            p[2] = color & 0xFF;
        }
        else
        {
            p[0] = color & 0xFF;
            p[1] = (color >> 8) & 0xFF;
            p[2] = (color >> 16) & 0xFF;
        }
    }
    else if (bpp == 2)
        *(Uint16*)p = color;
    else if (bpp == 1)
        *p = color;
}

// Verifica se duas cores são iguais
bool FloodFill::isSameColor(Uint32 color1, Uint32 color2)
{
    return color1 == color2;
}

// Preenche uma região usando Scanline Flood Fill
void FloodFill::fill(Point startPoint, Color newColor)
{
    SDL_Surface* surface = Context::getInstance()->getWindowSurface();

    if (surface == nullptr)
        return;

    int startX = startPoint.getX();
    int startY = startPoint.getY();

    // O Flood Fill funciona somente no Canvas
    if (startX < 0 || startX >= surface->w || startY < 40 || startY >= surface->h)
        return;

    Uint32 oldColor = getPixel(startX, startY);

    Uint32 fillColor = SDL_MapRGBA(
        surface->format,
        newColor.getR(),
        newColor.getG(),
        newColor.getB(),
        255
    );

    if (oldColor == fillColor)
        return;

    std::stack<Point> points;
    points.push(Point(startX, startY));

    while (!points.empty())
    {
        Point current = points.top();
        points.pop();

        int x = current.getX();
        int y = current.getY();

        // Ignora pontos fora do Canvas
        if (y < 40 || y >= surface->h)
            continue;

        // Procura o início da linha
        while (x >= 0 && getPixel(x, y) == oldColor)
            x--;

        x++;

        bool up = false;
        bool down = false;

        // Preenche toda a linha
        while (x < surface->w && getPixel(x, y) == oldColor)
        {
            setPixel(x, y, fillColor);

            // Verifica a linha de cima
            if (y > 40)
            {
                bool same = getPixel(x, y - 1) == oldColor;

                if (same && !up)
                {
                    points.push(Point(x, y - 1));
                    up = true;
                }
                else if (!same)
                    up = false;
            }

            // Verifica a linha de baixo
            if (y < surface->h - 1)
            {
                bool same = getPixel(x, y + 1) == oldColor;

                if (same && !down)
                {
                    points.push(Point(x, y + 1));
                    down = true;
                }
                else if (!same)
                    down = false;
            }

            x++;
        }
    }
}
