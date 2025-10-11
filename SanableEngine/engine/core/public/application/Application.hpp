#pragma once

#include <vector>
#include <optional>

#include "SDL_events.h"

#include <ReflectionSpec.hpp>
#include "MemoryHeap.hpp"

#include "../dllapi.h"

#include "WindowSettings.hpp"
#include "application/PluginManager.hpp"

namespace gpr460 { class System; }
class Game;
class Window;

class Application
{
protected:
    bool isAlive = false;
    gpr460::System* system = nullptr;
    std::optional<MemoryHeap> heap; //Optional so we can do late initialization/early destruction
    constexpr static size_t frameAllocatorSize = 4096;
    PluginManager pluginManager;
    friend class PluginManager;

    Window* mainWindow = nullptr;

public:
    ENGINECORE_API Application();
    ENGINECORE_API virtual ~Application();

    ENGINECORE_API virtual void init(gpr460::System& system) = 0;
    ENGINECORE_API virtual void doMainLoop() = 0;
    ENGINECORE_API virtual void frameStep() = 0;
    ENGINECORE_API virtual void cleanup() = 0;

    ENGINECORE_API gpr460::System* getSystem();
    ENGINECORE_API MemoryHeap* getHeap();
    ENGINECORE_API PluginManager* getPluginManager();
    ENGINECORE_API Window* getMainWindow();
    ENGINECORE_API const Window* getMainWindow() const;
    ENGINECORE_API void setMainWindow(Window*); // Note: does NOT destroy old main window

    ENGINECORE_API Window* buildWindow(WindowSettings& settings);
};
