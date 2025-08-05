#include "System_PlayInEditor.hpp"

#include <cassert>

System_PlayInEditor::System_PlayInEditor(gpr460::System* baseSystem) :
	baseSystem(baseSystem)
{
}

System_PlayInEditor::~System_PlayInEditor()
{
}

void System_PlayInEditor::DoMainLoop(void(*stepFn)(void*), void* stepArg)
{
	assert(false); // Yeah, don't do this
}

void System_PlayInEditor::DebugPause()
{
	baseSystem->DebugPause();
}

void System_PlayInEditor::ShowError(const std::wstring& message)
{
	baseSystem->ShowError(message;
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
	// TODO inline widget editor
	Window* window = baseSystem->createWindow(settings);
	windows.push_back(window);
	return window;
}

void System_PlayInEditor::destroyWindow(Window* window)
{
	baseSystem->destroyWindow(window);

	auto it = windows.find(window);
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
