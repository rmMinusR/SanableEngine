#pragma once

#include <vector>
#include "gui/Widget.hpp"

class PluginManager;
class PluginView;

class PluginManagerView : public Widget
{
	int selectionIndex;
	PluginManager* mgr = nullptr;
	Material* material;
	std::vector<PluginView*> pluginViews;

public:
	PluginManagerView(HUD* hud, PluginManager* mgr, Material* material);
	~PluginManagerView();

	virtual void tick() override;

	virtual const Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) override;
};
