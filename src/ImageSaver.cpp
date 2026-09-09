#include "ImageSaver.h"

#include <cstdio>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

long long ImageSaver::generateSaveId()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y%m%d%H%M%S");

    return std::stoll(oss.str());
}

bool ImageSaver::saveFile(SDL_Surface* surface, int x, int y, int width, int height)
{
    if (!surface)
    {
        printf("Superficie inválida.\n");
        return false;
    }
    // Salva somente a parte do canvas
    SDL_Surface* canvasSurface = SDL_CreateRGBSurface(0, width, height, 32, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    if (!canvasSurface)
    {
        printf("Erro ao criar superficie do Canvas: %s\n", SDL_GetError());
        return false;
    }

    SDL_Rect canvasRect = {x, y, width, height};

    if (SDL_BlitSurface(surface, &canvasRect, canvasSurface, nullptr) != 0)
    {
        printf("Erro ao copiar Canvas: %s\n", SDL_GetError());
        SDL_FreeSurface(canvasSurface);
        return false;
    }

    // Usar "HOME" se for Linux
    const char* home = getenv("USERPROFILE");

    if (home == nullptr)
    {
        printf("Não foi possível determinar o diretório do usuário.\n");
        SDL_FreeSurface(canvasSurface);
        return false;
    }

    std::string pictures = std::string(home) + "/Pictures";
    std::string name = pictures + "/imagem-" + std::to_string(generateSaveId()) + ".bmp";

    bool success = SDL_SaveBMP(canvasSurface, name.c_str()) == 0;

    if (success)
        printf("Arquivo salvo com sucesso: %s\n", name.c_str());
    else
        printf("Erro ao salvar: %s\n", SDL_GetError());

    SDL_FreeSurface(canvasSurface);

    return success;
}
