#pragma once

#include "application/Application.hpp"
#include "game/Level.hpp"
#include "dllapi.h"

class HUD;
class Game;
class System_PlayInEditor;

class EditorApplication : public Application
{
public:
    EDITORSHARED_API EditorApplication();
    EDITORSHARED_API virtual ~EditorApplication();

    EDITORSHARED_API virtual void init(gpr460::System& system) override;
    EDITORSHARED_API virtual void cleanup() override;
    EDITORSHARED_API void setupDefaultLayout(Window* editorWindow, HUD* hud);

    EDITORSHARED_API void doMainLoop();
    EDITORSHARED_API void frameStep();

    EDITORSHARED_API Game* getCurrentGame();

private:
    System_PlayInEditor* gameSystem = nullptr;
    Game* currentGame = nullptr;
    bool currentGamePaused = false;

    TypedMemoryPool<Level>* levels = nullptr; // May change upon entering/exiting PIE

    void startPlayInEditor(Game* game);
    void stopPlayInEditor();
};
