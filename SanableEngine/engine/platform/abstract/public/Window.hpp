#pragma once

#include <string>
#include <memory>
#include "math/Vector2.inl"

union SDL_Event;
struct SDL_Window;

class Renderer;
class WindowUserLogic;
struct WindowSettings;
template<typename T> struct thunk_utils;
namespace gpr460 { class System; }
class MenuBar;

class Window
{
protected:
	WindowUserLogic* userLogic;

	bool closeRequested;

	gpr460::System* system;
	void* context; // Application

	virtual void handleEvent(SDL_Event& ev);
	friend class Application;
	
	Window(const WindowSettings& settings, gpr460::System* system, void* context);
	virtual ~Window(); // Only System may call this
	friend struct thunk_utils<Window>;
public:
	virtual WindowUserLogic* getUserLogic();
	void* getContext() { return context; }
	gpr460::System* getSystem() { return system; }

	virtual void draw() const = 0;

	bool isFocused() const;
	virtual Renderer* getRenderer() = 0;
	virtual void setActiveDrawTarget() const = 0;

	virtual void move(int x, int y) = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual Vector2<int> getSize() const = 0;
	virtual bool wasCloseRequested() const = 0;

	virtual MenuBar* getMenuBar(bool create = false) = 0;
	virtual const MenuBar* getMenuBar() const = 0;

	virtual void setUserLogic(WindowUserLogic* v); //Note: Destroys old handler, if it exists
};
