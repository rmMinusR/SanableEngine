#pragma once

#include <vector>

#include "CallBatcher.inl"
#include "MemoryManager.hpp"
#include "StackAllocator.hpp"

#include "dllapi.h"
#include "PluginManager.hpp"
#include "WindowBuilder.hpp"

class ModuleTypeRegistry;
class GameObject;
class Component;
class IUpdatable;
class I3DRenderable;
namespace gpr460 { class System; }
class Renderer;

class EngineCore
{
    friend void engine_reportTypes(ModuleTypeRegistry* registry);

private:
    bool isAlive;
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

    CallBatcher<IUpdatable   > updateList;
    CallBatcher<I3DRenderable> _3dRenderList;
    friend class GameObject;

    std::vector<Window*> windows;
    friend class WindowBuilder;
    Window* mainWindow = nullptr;

    void processEvents();

    void refreshCallBatchers();

    void destroyImmediate(GameObject* go);
    void destroyImmediate(Component* c);

    void tick();
    void draw();

public:
    bool quit = false;
    int frame = 0;

    ENGINECORE_API EngineCore();
    ENGINECORE_API ~EngineCore();

    typedef void (*UserInitFunc)(EngineCore*);
    ENGINECORE_API void init(WindowBuilder& mainWindowBuilder, gpr460::System& system, UserInitFunc userInitCallback);
    ENGINECORE_API void shutdown();

    ENGINECORE_API GameObject* addGameObject();
    ENGINECORE_API void destroy(GameObject* go);

    ENGINECORE_API void doMainLoop();
    ENGINECORE_API static void frameStep(void* arg);

    ENGINECORE_API gpr460::System* getSystem();
    ENGINECORE_API MemoryManager* getMemoryManager();
    ENGINECORE_API StackAllocator* getFrameAllocator();
    ENGINECORE_API const CallBatcher<I3DRenderable>* get3DRenderables();
    ENGINECORE_API Renderer* getRenderer();

    ENGINECORE_API Window* getMainWindow();
    ENGINECORE_API WindowBuilder buildWindow(const std::string& name, int width, int height, std::unique_ptr<WindowRenderPipeline>&& renderPipeline);
};
