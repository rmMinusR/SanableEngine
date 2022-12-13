#pragma once

#include <vector>

#include "CallBatcher.inl"
#include "PluginManager.hpp"
#include "MemoryManager.hpp"

uint32_t GetTicks();

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
    PluginManager pluginManager;
    MemoryManager memoryManager;

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

    void reloadPlugins();
    void refreshCallBatchers();

public:
    SDL_Renderer* renderer = nullptr;
    uint32_t frameStart = 0;
    bool quit = false;
    int frame = 0;

    EngineCore();
    ~EngineCore();

    typedef void (*UserInitFunc)(EngineCore*);
    typedef gpr460::System* (*SystemFactoryFunc)();
    void init(char const* windowName, int windowWidth, int windowHeight, SystemFactoryFunc createSystem, UserInitFunc userInitCallback);
    void shutdown();

    GameObject* addGameObject();
    void destroy(GameObject* go);
    void destroyImmediate(GameObject* go);
    void destroyImmediate(Component* c);

    void doMainLoop();
    static void frameStep(void* arg);

    void tick();
    void draw();

    gpr460::System* getSystem() { return system; }
    MemoryManager* getMemoryManager() { return &memoryManager; }
};