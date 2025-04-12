#pragma once

#include <string>
#include <memory>

union SDL_Event;
struct SDL_Window;

class Renderer;
class WindowInputProcessor;
class WindowRenderPipeline;
struct WindowSettings;
template<typename T> struct thunk_utils;
namespace gpr460 { class System; }

class Window
{
protected:
	WindowRenderPipeline* renderPipeline;
	WindowInputProcessor* inputProcessor;
	SDL_Window* handle;

	bool closeRequested;

	gpr460::System* system;
	void* context; // Application
	void draw() const;

	virtual void handleEvent(SDL_Event& ev) = 0;
	friend class Application;
	
	Window(const WindowSettings& settings, gpr460::System* system, void* context);
	virtual ~Window();
	friend struct thunk_utils<Window>;
public:
	virtual WindowRenderPipeline* getRenderPipeline();
	virtual WindowInputProcessor* getInputProcessor();
	void* getContext() { return context; }
	gpr460::System* getSystem() { return system; }

	bool isFocused() const;
	virtual Renderer* getRenderer() = 0;
	virtual void setActiveDrawTarget() = 0;

	virtual void move(int x, int y) = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual bool wasCloseRequested() const = 0;

	virtual void setRenderPipeline(WindowRenderPipeline* v) = 0; //Note: Does NOT destroy old render pipeline, if it exists
	virtual void setInputProcessor(WindowInputProcessor* v) = 0; //Note: Does NOT destroy old input processor, if it exists
};
