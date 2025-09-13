#pragma once

#include <string>
#include <memory>
#include "math/Vector2.inl"

union SDL_Event;
struct SDL_Window;

class Application;
class Renderer;
class Framebuffer;
class WindowUserLogic;
struct WindowSettings;
template<typename T> struct thunk_utils;
namespace gpr460 { class System; }
class MenuBar;

class Window
{
protected:
	WindowUserLogic* userLogic;
	friend class Application;

	bool closeRequested = false;

	gpr460::System* system;
	Application* context;

	Window(const WindowSettings& settings, gpr460::System* system, Application* context);
	virtual ~Window(); // Only System may call this
	friend struct thunk_utils<Window>;
public:
	virtual WindowUserLogic* getUserLogic();
	Application* getContext() { return context; }
	gpr460::System* getSystem() { return system; }

	bool isFocused() const;
	virtual Renderer* getRenderer() = 0;
	virtual void setActiveDrawTarget() const = 0;
	virtual void draw() const = 0;

	virtual void move(int x, int y) = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual Vector2<int> getSize() const = 0;

	virtual void setSize(int w, int h) = 0;
	virtual void setUserResizable(bool val) = 0;

	virtual const Framebuffer* getFramebuffer() const = 0; // May be null
	virtual void setFramebuffer(Framebuffer*) = 0; // Takes ownership

	virtual bool wasCloseRequested() const = 0;
	virtual void requestClose();

	virtual MenuBar* getMenuBar(bool create = false) = 0;
	virtual const MenuBar* getMenuBar() const = 0;

	virtual void setUserLogic(WindowUserLogic* v); //Note: Destroys old handler, if it exists

	virtual void handleEvent(const SDL_Event& ev);
	virtual SDL_Window* getSdlHandle() const = 0;
};
