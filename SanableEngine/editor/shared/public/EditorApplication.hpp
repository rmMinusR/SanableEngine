#pragma once

#include <filesystem>

#include "application/Application.hpp"
#include "game/Level.hpp"
#include "dllapi.h"

class HUD;
class Game;
class System_PlayInEditor;

class EditorApplication : public Application
{
public:
    EDITORSHARED_API EditorApplication(std::filesystem::path projectDir);
    EDITORSHARED_API virtual ~EditorApplication();

    EDITORSHARED_API virtual void init(gpr460::System& system) override;
    EDITORSHARED_API virtual void cleanup() override;
    EDITORSHARED_API void setupDefaultLayout(Window* editorWindow, HUD* hud);

    EDITORSHARED_API void doMainLoop();
    EDITORSHARED_API void frameStep();

    EDITORSHARED_API std::filesystem::path getProjectDir();

    EDITORSHARED_API void startPlayInEditor();
    EDITORSHARED_API void stopPlayInEditor(bool force = false);
    EDITORSHARED_API Game* getCurrentGame();
    EDITORSHARED_API void tickCurrentGame();
    EDITORSHARED_API void setCurrentGamePaused(bool paused);
    EDITORSHARED_API bool isCurrentGamePaused() const;

private:
    System_PlayInEditor* gameSystem = nullptr;
    Game* currentGame = nullptr;
    void(*currentGameStepFn)(Application*);
    bool currentGamePaused = false;

    std::filesystem::path projectDir;

    TypedMemoryPool<Level>* levels = nullptr; // May change upon entering/exiting PIE

    // Standalone call path:
    //   SDL_main
    //   SanableMain
    //   Game::doMainLoop
    //   System::DoMainLoop
    //   Game::frameStep
    //
    // PIE call path:
    //   SDL_main
    //   SanableMain [editor]
    //   EditorApplication::doMainLoop
    //   System::DoMainLoop [platform]
    //   EditorApplication::frameStep
    //           V
    //   [PIE start via UI]
    //   EditorApplication::startPlayInEditor
    //   SanableMain [game DLL]
    //   Game::doMainLoop
    //   System_PlayInEditor::doMainLoop
    //     sets EditorApplication::currentGame
    //     thread suspends until quit is requested
    //   [via EditorApplication::frameStep] EditorApplication::currentGame->frameStep
    friend class System_PlayInEditor;
};
