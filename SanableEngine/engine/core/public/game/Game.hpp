#pragma once

#include <vector>
#include <optional>
#include <functional>
#include "../dllapi.h"
#include "TypedMemoryPool.hpp"
#include "StackAllocator.hpp"
#include "System.hpp"
#include "application/Application.hpp"
#include "Level.hpp"

class PluginManager;
class GameObject;
class InputSystem;
class GameWindowRenderPipeline;

class Game : public Application
{
    InputSystem* inputSystem;

    StackAllocator frameAllocator; //Temp memory that will be reset every frame
    TypedMemoryPool<Level>* levels;
    void applyConcurrencyBuffers(); //Passthrough for now
    friend class GameWindowRenderPipeline;
    
    friend class PluginManager;
    void refreshCallBatchers(bool force = false);

    //TODO allow accumulation from multiple heaps to allow collection from multiple levels
    //PoolCallBatcher<IUpdatable> updateList;
    //PoolCallBatcher<I3DRenderable> _3dRenderList;

public:
    ENGINECORE_API Game();
    ENGINECORE_API virtual ~Game();

    ENGINECORE_API virtual void init(gpr460::System& system) override;
    ENGINECORE_API virtual void cleanup() override;
    ENGINECORE_API void tick();
    int frame = 0;

	ENGINECORE_API InputSystem* getInput();
    ENGINECORE_API StackAllocator* getFrameAllocator();

	ENGINECORE_API void visitLevels(const std::function<void(Level*)>& visitor);
	ENGINECORE_API Level* getLevel(size_t which);
	ENGINECORE_API size_t getLevelCount() const;
	ENGINECORE_API Level* addLevel();
	ENGINECORE_API void removeLevel(Level* level);

    ENGINECORE_API void doMainLoop();
    ENGINECORE_API void frameStep();
};
