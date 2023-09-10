#pragma once

#include "Component.hpp"
#include "StandardReflectionHooks.hpp"

class PluginManager;

class PluginManagerView : public Component, public IRenderable, public IUpdatable
{
	SANABLE_REFLECTION_HOOKS

	int selectionIndex;
	PluginManager* mgr = nullptr;

public:
	PluginManagerView();
	~PluginManagerView();

	virtual void onStart() override;
	virtual void Update() override;
	virtual void Render(Renderer* target) override;
};
