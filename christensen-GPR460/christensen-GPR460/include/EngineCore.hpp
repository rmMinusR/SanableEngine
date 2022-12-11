#pragma once

#include <vector>
#include <SDL.h>

#include "System.hpp"
#include "CallBatcher.inl"
#include "PluginManager.hpp"

Uint32 GetTicks();

class GameObject;

class IUpdatable;
class IRenderable;

class EngineCore
{
private:
    bool isAlive;
    SDL_Window* window = nullptr;
    gpr460::System system;
    PluginManager pluginManager;

    std::vector<GameObject*> objects;

    CallBatcher<IUpdatable > updateList;
    CallBatcher<IRenderable> renderList;

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

    void doMainLoop();
    static void frameStep(void* arg);

    void tick();
    void draw();

    CallBatcher<IUpdatable >* getUpdatables () { return &updateList; }
    CallBatcher<IRenderable>* getRenderables() { return &renderList; }
    gpr460::System* getSystem() { return &system; }
};

extern EngineCore engine; //FIXME singleton bad