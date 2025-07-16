#pragma once

#include <Windows.h>

#include <SDL_video.h>
#include "Window.hpp"
#include "OpenGlRenderer.hpp"

struct GLSettings;
template<typename T> struct thunk_utils;
namespace gpr460 { class System_Win32; }
class MenuBar_Win32;

class Window_Win32 : public Window
{
private:
	SDL_Window* sdlHandle;
	OpenGlRenderer renderer;
	MenuBar_Win32* menuBar = nullptr;

	int sdlID;

	Window_Win32(const WindowSettings& settings, Application* engine, SDL_Window* handle);
	virtual ~Window_Win32();
	friend class gpr460::System_Win32;
	friend struct thunk_utils<Window_Win32>;
public:

	virtual Renderer* getRenderer() override { return &renderer; }
	virtual void setActiveDrawTarget() const override;
	void draw() const override;

	virtual void move(int x, int y) override;
	virtual int getWidth() const override;
	virtual int getHeight() const override;
	virtual Vector2<int> getSize() const override;
	virtual bool wasCloseRequested() const override;

	virtual MenuBar* getMenuBar(bool create = false) override;
	virtual const MenuBar* getMenuBar() const override;

	// PLATFORM SPECIFIC

	HWND getNativeHandle();
private:
	void handleNativeEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
