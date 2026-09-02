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

    if (surface == nullptr ||
        x < 0 || x >= surface->w ||
        y < 0 || y >= surface->h)
    {
        return 0;
    }

    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16*)p;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];

            return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32*)p;

        default:
            return 0;
    }
}

// Altera a cor de um pixel
void FloodFill::setPixel(int x, int y, Color color)
{
    SDL_Surface* surface = Context::getInstance()->getWindowSurface();

    if (surface == nullptr ||
        x < 0 || x >= surface->w ||
        y < 40 || y >= surface->h)
    {
        return;
    }

    Uint32 pixelColor = SDL_MapRGBA(
        surface->format,
        color.getR(),
        color.getG(),
        color.getB(),
        255
    );

    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            *p = pixelColor;
            break;

        case 2:
            *(Uint16*)p = pixelColor;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixelColor >> 16) & 0xFF;
                p[1] = (pixelColor >> 8) & 0xFF;
                p[2] = pixelColor & 0xFF;
            }
            else
            {
                p[0] = pixelColor & 0xFF;
                p[1] = (pixelColor >> 8) & 0xFF;
                p[2] = (pixelColor >> 16) & 0xFF;
            }
            break;

        case 4:
            *(Uint32*)p = pixelColor;
            break;
    }
}

// Verifica se duas cores são iguais
bool FloodFill::isSameColor(Uint32 color1, Uint32 color2)
{
    return color1 == color2;
}

// Preenche uma regiao
void FloodFill::fill(Point startPoint, Color newColor)
{
    SDL_Surface* surface = Context::getInstance()->getWindowSurface();

    if (surface == nullptr)
        return;

    int startX = startPoint.getX();
    int startY = startPoint.getY();

    // O Flood Fill funciona somente no Canvas
    if (startX < 0 || startX >= surface->w ||
        startY < 40 || startY >= surface->h)
    {
        return;
    }

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
    points.push(startPoint);

    while (!points.empty())
    {
        Point current = points.top();
        points.pop();

        int x = current.getX();
        int y = current.getY();

        // Verifica os limites do Canvas
        if (x < 0 || x >= surface->w ||
            y < 40 || y >= surface->h)
        {
            continue;
        }

        // Ignora pixels de outra cor
        if (!isSameColor(getPixel(x, y), oldColor))
            continue;

        setPixel(x, y, newColor);

        // Adiciona os quatro vizinhos
        points.push(Point(x + 1, y));
        points.push(Point(x - 1, y));
        points.push(Point(x, y + 1));
        points.push(Point(x, y - 1));
    }
}
