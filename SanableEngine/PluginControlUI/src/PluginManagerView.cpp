#include "PluginManagerView.hpp"

#include "application/PluginManager.hpp"
#include "application/Plugin.hpp"
#include "Renderer.hpp"
#include "game/GameObject.hpp"
#include "game/Game.hpp"
#include "application/Application.hpp"

PluginManagerView::PluginManagerView()
{
}

PluginManagerView::~PluginManagerView()
{
}

void PluginManagerView::onStart()
{
	mgr = gameObject->getContext()->getApplication()->getPluginManager();
}

void PluginManagerView::Update()
{
	
}

Material* PluginManagerView::getMaterial() const
{
	return nullptr;
}

void PluginManagerView::renderImmediate(Renderer* target) const
{
	mgr->enumeratePlugins([&](Plugin* p)
	{
		//TODO implement
	});
}
