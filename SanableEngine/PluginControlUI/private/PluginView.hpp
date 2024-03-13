#pragma once

#include <ReflectionSpec.hpp>
#include "gui/Widget.hpp"

struct Plugin;
class LabelWidget;

class PluginView : public Widget
{
	Plugin* plugin;

	LabelWidget* path;
	LabelWidget* name;
	LabelWidget* status;
	//TODO manual control buttons
	//TODO RTTI status + details button

	void tryInit();

public:
	PluginView(HUD* hud);
	~PluginView();

	void setViewed(Plugin* plugin);

	virtual void tick() override;

	virtual const Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) override;
};
