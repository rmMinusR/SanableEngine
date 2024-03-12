#pragma once

#include "gui/Widget.hpp"

struct Plugin;
class LabelWidget;

class PluginView : public Widget
{
	Plugin* plugin;

	LabelWidget* path;
	LabelWidget* name;
	LabelWidget* status;

public:
	PluginView(HUD* hud);
	~PluginView();

	void setViewed(Plugin* plugin);

	virtual void tick() override;

	virtual const Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) override;
};
