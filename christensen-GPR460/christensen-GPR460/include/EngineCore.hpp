#pragma once

#include <vector>
#include <SDL.h>

#include "System.hpp"

Uint32 GetTicks();

class GameObject;

class EngineCore
{
private:
    bool isAlive;
    SDL_Window* window = nullptr;
    gpr460::System system;

    std::vector<GameObject*> objects; //FIXME change this to an array or a pooling structure

    void processEvents();

public:
    SDL_Renderer* renderer = nullptr;
    Uint32 frameStart = 0;
    bool quit = false;
    int frame = 0;

    EngineCore();
    ~EngineCore();

    typedef void (*UserInitFunc)(EngineCore*);
    void init(char const* windowName, int windowWidth, int windowHeight, UserInitFunc userInitCallback);
    void shutdown();

    GameObject* addGameObject();

    void tick();
    void draw();
};

extern EngineCore engine; //FIXME singleton bad