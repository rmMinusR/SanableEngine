#pragma once

#include "dllapi.h"

#include <vector>

#include "CallBatcher.inl"
#include "PluginManager.hpp"
#include "MemoryManager.hpp"

struct SDL_Window;
struct SDL_Renderer;

class GameObject;
class Component;

class IUpdatable;
class IRenderable;

namespace gpr460 { class System; }

class EngineCore
{
private:
    bool isAlive;
    SDL_Window* window = nullptr;
    gpr460::System* system;
    MemoryManager memoryManager;
    PluginManager pluginManager;
    friend class PluginManager;

    std::vector<GameObject*> objects;
    void applyConcurrencyBuffers();
    std::vector<GameObject*> objectAddBuffer;
    std::vector<GameObject*> objectDelBuffer;
    std::vector<std::pair<Component*, GameObject*>> componentAddBuffer;
    std::vector<Component*> componentDelBuffer;

    CallBatcher<IUpdatable > updateList;
    CallBatcher<IRenderable> renderList;
    friend class GameObject;

    void processEvents();

    void refreshCallBatchers();

    void destroyImmediate(GameObject* go);
    void destroyImmediate(Component* c);

    void tick();
    void draw();

public:
    SDL_Renderer* renderer = nullptr;
    uint32_t frameStart = 0;
    bool quit = false;
    int frame = 0;

    ENGINECORE_API EngineCore();
    ENGINECORE_API ~EngineCore();

    typedef void (*UserInitFunc)(EngineCore*);
    typedef gpr460::System* (*SystemFactoryFunc)();
    ENGINECORE_API void init(char const* windowName, int windowWidth, int windowHeight, SystemFactoryFunc createSystem, UserInitFunc userInitCallback);
    ENGINECORE_API void shutdown();

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);

    ENGINECORE_API void doMainLoop();
    ENGINECORE_API static void frameStep(void* arg);

    gpr460::System* getSystem() { return system; }
    MemoryManager* getMemoryManager() { return &memoryManager; }
};