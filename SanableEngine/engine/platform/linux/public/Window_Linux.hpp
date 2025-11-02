#pragma once

#include <Windows.h>

#include <SDL_video.h>
#include "Window.hpp"
#include "OpenGlRenderer.hpp"

struct GLSettings;
template<typename T> struct thunk_utils;
namespace gpr460 { class System_Linux; }
class MenuBar_Linux;

class Window_Linux : public Window
{
private:
	SDL_Window* sdlHandle;
	OpenGlRenderer renderer;
	MenuBar_Linux* menuBar = nullptr;

	int sdlID;

	Window_Linux(const WindowSettings& settings, Application* engine, SDL_Window* handle);
	virtual ~Window_Linux();
	friend class gpr460::System_Linux;
	friend struct thunk_utils<Window_Linux>;
public:

	virtual Renderer* getRenderer() override { return &renderer; }
	virtual void setActiveDrawTarget() const override;
	virtual void draw() const override;

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
