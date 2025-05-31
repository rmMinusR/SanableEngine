#pragma once

#include <future>

#include <ReflectionSpec.hpp>
#include "gui/Widget.hpp"
#include "application/Plugin.hpp"

class PluginManager;
class LabelWidget;
class ButtonWidget;
class ImageWidget;

class PluginView : public Widget
{
	PluginManager* mgr = nullptr;
	Plugin* plugin = nullptr;

	LabelWidget* path = nullptr;
	LabelWidget* name = nullptr;
	LabelWidget* status = nullptr;

	ButtonWidget* btnDevRebuild = nullptr;
	ImageWidget* imgDevRebuildBg = nullptr;
	LabelWidget* lblDevRebuild = nullptr;
	
	ButtonWidget* btnToggleLoaded = nullptr;
	ImageWidget* imgToggleLoadedBg = nullptr;
	LabelWidget* lblToggleLoaded = nullptr;

	ButtonWidget* btnToggleHooked = nullptr;
	ImageWidget* imgToggleHookedBg = nullptr;
	LabelWidget* lblToggleHooked = nullptr;
	
	ButtonWidget* btnInspectTypes = nullptr;
	ImageWidget* imgInspectTypesBg = nullptr;
	LabelWidget* lblInspectTypes = nullptr;

	std::future<bool> buildTask;

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
