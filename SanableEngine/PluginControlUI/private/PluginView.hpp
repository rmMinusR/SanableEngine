#pragma once

#include <ReflectionSpec.hpp>
#include "gui/Widget.hpp"
#include "application/Plugin.hpp"

class PluginManager;
class LabelWidget;
class ButtonWidget;
class ImageWidget;

class PluginView : public Widget
{
	PluginManager* mgr;
	Plugin* plugin;
	Plugin::Status lastKnownStatus;

	LabelWidget* path;
	LabelWidget* name;
	LabelWidget* status;
	
	ButtonWidget* btnToggleLoaded;
	ImageWidget* imgToggleLoadedBg;
	LabelWidget* lblToggleLoaded;

	ButtonWidget* btnToggleHooked;
	ImageWidget* imgToggleHookedBg;
	LabelWidget* lblToggleHooked;
	
	ButtonWidget* btnInspectTypes;
	ImageWidget* imgInspectTypesBg;
	LabelWidget* lblInspectTypes;

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
