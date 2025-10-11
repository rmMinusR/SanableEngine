#pragma once

#include <vector>
#include <SDL_events.h>

#include "System.hpp"
#include "dllapi.h"

class EditorApplication;
class WidgetAsWindow;

class System_PlayInEditor : public gpr460::System
{
public:
	EDITORSHARED_API System_PlayInEditor(gpr460::System* baseSystem, EditorApplication* editor);
	EDITORSHARED_API virtual ~System_PlayInEditor();

	EDITORSHARED_API virtual void Init() override;
	EDITORSHARED_API virtual void DoMainLoop(void(*stepFn)(Application*), Application* app) override;
	EDITORSHARED_API virtual void Shutdown() override;

	EDITORSHARED_API virtual void pumpEvents() override;

	EDITORSHARED_API virtual bool isFocused(const Window*) override;
	EDITORSHARED_API virtual Window* createWindow(const WindowSettings& settings, Application* engine) override;
	EDITORSHARED_API virtual void destroyWindow(Window* window) override;
	EDITORSHARED_API virtual size_t getNumWindows() const override;
	EDITORSHARED_API virtual Window* getWindow(size_t which) override;

	EDITORSHARED_API virtual void DebugPause() override;

	//TODO these should be combined
	EDITORSHARED_API virtual void ShowError(const std::wstring& message) override;
	EDITORSHARED_API virtual void LogToErrorFile(const std::wstring& message) override;

	EDITORSHARED_API virtual std::vector<std::filesystem::path> ListPlugins(std::filesystem::path path) const override;
	EDITORSHARED_API virtual std::filesystem::path GetBaseDir() const override;

private:
	EditorApplication* editor;
	gpr460::System* baseSystem;

	std::vector<Window*> windows;
	std::vector<SDL_Event> bufferedEvents;

	Window* currentFocus = nullptr;
	WidgetAsWindow* playInEditorView = nullptr;
};
