#ifndef IMAGESAVER_H
#define IMAGESAVER_H

#include <SDL2/SDL.h>
#include <string>

class ImageSaver
{
public:
    long long generateSaveId();
    bool saveFile(SDL_Surface* surface, int x, int y, int width, int height);
};

#endif
