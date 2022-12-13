#pragma once

#include <vector>
#include <unordered_map>
#include <SDL.h>

#include "System.hpp"
#include "CallBatcher.inl"
#include "PluginManager.hpp"

Uint32 GetTicks();

class GameObject;
class Component;

class IUpdatable;
class IRenderable;

class EngineCore
{
    //FIXME singleton bad
private:
    static EngineCore* engine;
public:
    static void initInstance();
    inline static EngineCore* getInstance() { return EngineCore::engine; }
    static void cleanupInstance();

private:
    bool isAlive;
    SDL_Window* window = nullptr;
    gpr460::System system;
    PluginManager pluginManager;

    std::vector<GameObject*> objects;
    void applyConcurrencyBuffers();
    std::vector<GameObject*> objectAddBuffer;
    std::vector<GameObject*> objectDelBuffer;
    std::unordered_map<Component*, GameObject*> componentAddBuffer;
    std::vector<Component*> componentDelBuffer;

    CallBatcher<IUpdatable > updateList;
    CallBatcher<IRenderable> renderList;
    friend class GameObject;

    void processEvents();

    void reloadPlugins();
    void refreshCallBatchers();

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
    void destroy(GameObject* go);
    void destroyImmediate(GameObject* go);
    void destroyImmediate(Component* c);

    void doMainLoop();
    static void frameStep(void* arg);

    void tick();
    void draw();

    gpr460::System* getSystem() { return &system; }
};