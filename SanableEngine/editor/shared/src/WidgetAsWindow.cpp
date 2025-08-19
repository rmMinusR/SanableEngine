#include "WidgetAsWindow.hpp"

#include "gui/HUD.hpp"
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "WindowSettings.hpp"
#include "WindowUserLogic.hpp"

WidgetAsWindow::WidgetAsWindow(HUD* hud, const Material* widgetMaterial, const UISprite* inactiveSprite, WindowSettings&& settings, gpr460::System* system, Application* innerApplication) :
	ImageWidget(hud, widgetMaterial, inactiveSprite),
	Window(settings, system, innerApplication),
	renderProxy(hud->getWindow()->getRenderer(), this),
	inactiveSprite(inactiveSprite)
{
	Framebuffer::Settings framebufferSettings;
	renderTexture = hud->getWindow()->getRenderer()->newFramebuffer(settings.size, framebufferSettings);
	activeSprite = new UISprite3x3(renderTexture->getTexture());
	activeSprite->set({ 1, 1 }, { 0,0 }); // Full stretch
	activeSprite->set({ 2, 2 }, { 1,1 });
}

WidgetAsWindow::~WidgetAsWindow()
{
	delete renderTexture;
	delete activeSprite;
}

void WidgetAsWindow::setLive(bool live)
{
	ImageWidget::setSprite(live ? inactiveSprite : activeSprite);
}

Renderer* WidgetAsWindow::getRenderer()
{
	return &renderProxy;
}

void WidgetAsWindow::setActiveDrawTarget() const
{
	hud->getWindow()->setActiveDrawTarget();
}

// VERY ugly hack to mock render call
#include "public_cast.hpp"
PUBLIC_CAST_GIVE_BOUND_FN_ACCESS(WindowUserLogic_render, WindowUserLogic, render, void, Rect<float>);

void WidgetAsWindow::draw() const
{
	// Long winded, access specifier-ignoring version of:
	// userLogic->render({ {0,0}, size });
	auto render_fn = DO_PUBLIC_CAST(WindowUserLogic_render);
	(userLogic->*render_fn)({ {0,0}, getTransform()->getRect().size });
}

void WidgetAsWindow::move(int x, int y)
{
	// Yeah, that's not happening
}

Vector2<int> WidgetAsWindow::getSize() const
{
	return getTransform()->getRect().size.convert<int>();
}

bool WidgetAsWindow::wasCloseRequested() const
{
	return false; // Widget is also uncloseable in tab view
}

int WidgetAsWindow::getWidth() const
{
	return getSize().x;
}

int WidgetAsWindow::getHeight() const
{
	return getSize().y;
}

MenuBar* WidgetAsWindow::getMenuBar(bool create)
{
	return nullptr; // TODO implement
}

const MenuBar* WidgetAsWindow::getMenuBar() const
{
	return nullptr; // TODO implement
}

SDL_Window* WidgetAsWindow::getSdlHandle() const
{
	return nullptr;
}
