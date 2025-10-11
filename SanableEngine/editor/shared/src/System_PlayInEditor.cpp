#include "System_PlayInEditor.hpp"

#include <cassert>

#include "Window.hpp"
#include "game/Game.hpp"
#include "EditorApplication.hpp"
#include "WidgetAsWindow.hpp"

System_PlayInEditor::System_PlayInEditor(gpr460::System* baseSystem, EditorApplication* editor) :
	baseSystem(baseSystem),
	editor(editor)
{
}

System_PlayInEditor::~System_PlayInEditor()
{
}

void System_PlayInEditor::Init()
{
	bufferedEvents.clear();
}

void System_PlayInEditor::DoMainLoop(void(*stepFn)(Application*), Application* app)
{
	editor->currentGame = static_cast<Game*>(app);
	editor->currentGameStepFn = stepFn;
	editor->currentGamePaused = false;
}

void System_PlayInEditor::Shutdown()
{
	bufferedEvents.clear();

	editor->currentGame = nullptr;
	editor->currentGameStepFn = nullptr;
}

void System_PlayInEditor::DebugPause()
{
	baseSystem->DebugPause();
}

void System_PlayInEditor::ShowError(const std::wstring& message)
{
	baseSystem->ShowError(message);
}

void System_PlayInEditor::LogToErrorFile(const std::wstring& message)
{
	baseSystem->LogToErrorFile(message);
}

bool System_PlayInEditor::isFocused(const Window* window)
{
	return baseSystem->isFocused(window);
}

Window* System_PlayInEditor::createWindow(const WindowSettings& settings, Application* engine)
{
	Window* window = nullptr;

	if (windows.size() == 0)
	{
		// First window is assumed to be main, embed in layout
		window = playInEditorView;
		playInEditorView->setLive(true);
	}
	else
	{
		window = baseSystem->createWindow(settings, engine);
	}

	windows.push_back(window);
	return window;
}

void System_PlayInEditor::destroyWindow(Window* window)
{
	if (WidgetAsWindow* widgetWindow = dynamic_cast<WidgetAsWindow*>(window))
	{
		widgetWindow->setLive(false);
	}
	else
	{
		baseSystem->destroyWindow(window);
	}

	auto it = std::find(windows.begin(), windows.end(), window);
	assert(it != windows.end());
	windows.erase(it);
}

size_t System_PlayInEditor::getNumWindows() const
{
	return windows.size();
}

Window* System_PlayInEditor::getWindow(size_t which)
{
	return windows[which];
}

void System_PlayInEditor::pumpEvents()
{
	//Utility functions
	auto lookupWindow = [&](SDL_Window* windowHandle)
	{
		// Translate editor main window -> sandboxed main window
		if (baseSystem->getWindow(0)->getSdlHandle() == windowHandle)
		{
			return windows[0];
		}

		// Lookup for non-main windows
		auto it = std::find_if(windows.begin(), windows.end(), [=](Window* w) { return w->getSdlHandle() == windowHandle; });
		if (it != windows.end()) return *it;
		else return (Window*)nullptr;
	};
	auto forwardToWindow = [&](SDL_Window* windowHandle, const SDL_Event& event)
	{
		Window* w = lookupWindow(windowHandle);
		if (w) (w)->handleEvent(event);
	};

	for(const SDL_Event& ev : bufferedEvents)
	{
		//Forward events to appropriate windows
		switch (ev.type)
		{
			// Passthrough input
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			forwardToWindow(SDL_GetKeyboardFocus(), ev);
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			forwardToWindow(SDL_GetMouseFocus(), ev);
			break;

			//Window focus management
		case SDL_WINDOWEVENT:
		{
			Window* window = lookupWindow(SDL_GetWindowFromID(ev.window.windowID));
			if (window) // Null if events happened before destruction but after last pump
			{
				switch (ev.window.event)
				{
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					currentFocus = window;
					break;

				case SDL_WINDOWEVENT_FOCUS_LOST:
					if (currentFocus == window) currentFocus = nullptr;
					break;

				case SDL_WINDOWEVENT_CLOSE:
					window->requestClose();
					break;
				}
				window->handleEvent(ev);
				break;
			}
		}

		}
	}

	bufferedEvents.clear();
}

std::vector<std::filesystem::path> System_PlayInEditor::ListPlugins(std::filesystem::path path) const
{
	std::vector<std::filesystem::path> plugins = baseSystem->ListPlugins(path);
	// TODO filter out editor-only plugins
	return plugins;
}

std::filesystem::path System_PlayInEditor::GetBaseDir() const
{
	return editor->getProjectDir();
}
