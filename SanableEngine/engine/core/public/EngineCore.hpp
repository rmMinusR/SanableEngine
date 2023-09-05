#pragma once

#include <vector>

#include "CallBatcher.inl"
#include "MemoryManager.hpp"
#include "StackAllocator.hpp"
#include "Window.hpp"

#include "dllapi.h"
#include "PluginManager.hpp"

class ModuleTypeRegistry;
class GameObject;
class Component;
class IUpdatable;
class IRenderable;
namespace gpr460 { class System; }

class EngineCore
{
    friend void engine_reportTypes(ModuleTypeRegistry* registry);

private:
    bool isAlive;
    SDL_Window* window = nullptr;
    gpr460::System* system;
    MemoryManager memoryManager;
    StackAllocator frameAllocator; //Temp memory that will be reset every frame
    constexpr static size_t frameAllocatorSize = 4096;
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

    Window* mainWindow = nullptr;
public:
    bool quit = false;
    int frame = 0;

    ENGINECORE_API EngineCore();
    ENGINECORE_API ~EngineCore();

    typedef void (*UserInitFunc)(EngineCore*);
    ENGINECORE_API void init(char const* windowName, int windowWidth, int windowHeight, gpr460::System& system, UserInitFunc userInitCallback);
    ENGINECORE_API void shutdown();

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);

    ENGINECORE_API void doMainLoop();
    ENGINECORE_API static void frameStep(void* arg);

    ENGINECORE_API gpr460::System* getSystem();
    ENGINECORE_API MemoryManager* getMemoryManager();
    ENGINECORE_API StackAllocator* getFrameAllocator();
};