#pragma once

#include "game/Component.hpp"

class PluginManager;

class PluginManagerView : public Component, public I3DRenderable, public IUpdatable
{
	int selectionIndex;
	PluginManager* mgr = nullptr;

public:
	PluginManagerView();
	~PluginManagerView();

	virtual void onStart() override;
	virtual void Update() override;
	virtual Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) const override;
};
