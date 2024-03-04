#pragma once

#include "game/Component.hpp"

class PluginManager;
class ShaderProgram;
class Material;

class PluginManagerView : public Component, public I3DRenderable, public IUpdatable
{
	int selectionIndex;
	PluginManager* mgr = nullptr;
	Material* material;

public:
	PluginManagerView(Material* material);
	~PluginManagerView();

	virtual void onStart() override;
	virtual void Update() override;
	virtual Material* getMaterial() const override;
	virtual void renderImmediate(Renderer* target) const override;
};
