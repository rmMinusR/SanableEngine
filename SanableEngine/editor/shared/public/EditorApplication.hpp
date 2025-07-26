#pragma once

#include "application/Application.hpp"
#include "dllapi.h"

class HUD;
class Game;

class EditorApplication : public Application
{
public:
    EDITORSHARED_API EditorApplication(gpr460::System& system);
    EDITORSHARED_API virtual ~EditorApplication();

    EDITORSHARED_API virtual void init() override;
    EDITORSHARED_API virtual void cleanup() override;
    EDITORSHARED_API void setupDefaultLayout(Window* editorWindow, HUD* hud);

    EDITORSHARED_API void doMainLoop();
    EDITORSHARED_API void frameStep();

    EDITORSHARED_API Game* getCurrentGame();

private:
    // System_PIE* gameSystem = nullptr;
    Game* currentGame = nullptr;
};
