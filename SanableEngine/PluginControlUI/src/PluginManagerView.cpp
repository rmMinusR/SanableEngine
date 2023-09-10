#include "PluginManagerView.hpp"

#include "PluginManager.hpp"
#include "Plugin.hpp"
#include "Renderer.hpp"
#include "GameObject.hpp"
#include "EngineCore.hpp"

PluginManagerView::PluginManagerView()
{
}

PluginManagerView::~PluginManagerView()
{
}

void PluginManagerView::onStart()
{
	mgr = gameObject->getEngine()->getPluginManager();
}

void PluginManagerView::Update()
{
	
}

void PluginManagerView::Render(Renderer* target)
{
	for (Plugin* p : mgr->getPlugins())
	{
		
	}
}
