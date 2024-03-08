#include "PluginManagerView.hpp"

#include "application/PluginManager.hpp"
#include "application/Plugin.hpp"
#include "Renderer.hpp"
#include "Material.hpp"
#include "game/GameObject.hpp"
#include "game/Game.hpp"
#include "application/Application.hpp"

PluginManagerView::PluginManagerView(PluginManager* mgr, Material* material) :
	material(material),
	mgr(mgr)
{
}

PluginManagerView::~PluginManagerView()
{
}

const Material* PluginManagerView::getMaterial() const
{
	return material;
}

void PluginManagerView::renderImmediate(Renderer* target)
{
	mgr->enumeratePlugins([&](Plugin* p)
	{
		//TODO implement
	});
}
