#pragma once

#include <ReflectionSpec.hpp>
#include "gui/Widget.hpp"

struct Plugin;
class PluginManager;
class LabelWidget;
class ButtonWidget;
class ImageWidget;

class PluginView : public Widget
{
	PluginManager* mgr;
	Plugin* plugin;

	LabelWidget* path;
	LabelWidget* name;
	LabelWidget* status;
	
	ButtonWidget* btnToggleLoaded;
	ButtonWidget* btnToggleHooked;
	ImageWidget* imgToggleLoadedBg;
	ImageWidget* imgToggleHookedBg;
	LabelWidget* lblToggleLoaded;
	LabelWidget* lblToggleHooked;
	//TODO manual control buttons
	//TODO RTTI status + details button

	void tryInit();

public:
	PluginView(HUD* hud);
	~PluginView();

	void setViewed(Plugin* plugin, PluginManager* mgr);

	virtual void tick() override;

	virtual const Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) override;
};
