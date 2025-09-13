#pragma once

#include "gui/ImageWidget.hpp"
#include "gui/UISprite.hpp"
#include "Window.hpp"
#include "dllapi.h"
#include "WidgetAsWindow_RenderProxy.hpp"

class Framebuffer;

class WidgetAsWindow : public ImageWidget, public Window
{
	Framebuffer* renderTexture; // Owning
	WidgetAsWindow_RenderProxy renderProxy;
	const UISprite* inactiveSprite; // Non-owning
	UISprite3x3* activeSprite; // Owning

public:
	EDITORSHARED_API WidgetAsWindow(HUD* hud, const Material* widgetMaterial, const UISprite* inactiveSprite, WindowSettings&& settings, gpr460::System* system, Application* innerApplication);
	EDITORSHARED_API virtual ~WidgetAsWindow();

	EDITORSHARED_API void setLive(bool live);

	// Window

	EDITORSHARED_API virtual Renderer* getRenderer() override;
	EDITORSHARED_API virtual void setActiveDrawTarget() const override;
	EDITORSHARED_API virtual void draw() const override;

	EDITORSHARED_API virtual void move(int x, int y) override;
	EDITORSHARED_API virtual int getWidth() const override;
	EDITORSHARED_API virtual int getHeight() const override;
	EDITORSHARED_API virtual Vector2<int> getSize() const override;

	EDITORSHARED_API virtual const Framebuffer* getFramebuffer() const override; // May be null
	EDITORSHARED_API virtual void setFramebuffer(Framebuffer*) override; // Takes ownership

	EDITORSHARED_API virtual bool wasCloseRequested() const override;

	EDITORSHARED_API virtual MenuBar* getMenuBar(bool create = false) override;
	EDITORSHARED_API virtual const MenuBar* getMenuBar() const override;

	EDITORSHARED_API virtual SDL_Window* getSdlHandle() const override;
};
