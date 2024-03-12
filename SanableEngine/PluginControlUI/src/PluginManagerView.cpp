#include "PluginManagerView.hpp"

#include "application/PluginManager.hpp"
#include "Renderer.hpp"
#include "Material.hpp"
#include "game/GameObject.hpp"
#include "game/Game.hpp"
#include "application/Application.hpp"
#include "gui/HUD.hpp"
#include "PluginView.hpp"

PluginManagerView::PluginManagerView(HUD* hud, PluginManager* mgr, Material* material) :
	Widget(hud),
	mgr(mgr),
	material(material)
{
}

PluginManagerView::~PluginManagerView()
{
}

void PluginManagerView::tick()
{
	std::vector<Plugin*> plugins;
	mgr->enumeratePlugins([&](Plugin* p) { plugins.push_back(p); });

	//Ensure we have the right number of PluginViews
	while (pluginViews.size() < plugins.size())
	{
		PluginView* view = hud->addWidget<PluginView>();
		view->transform.snapToCorner({ 0, 0 }, Vector2f(0, 100));
		view->transform.fillParentX();
		pluginViews.push_back(view);
	}
	while (pluginViews.size() > plugins.size())
	{
		auto it = pluginViews.end()-1;
		hud->removeWidget(*it);
		pluginViews.erase(it);
	}

	//Update plugin views
	for (int i = 0; i < plugins.size(); ++i)
	{
		pluginViews[i]->setViewed(plugins[i]);
		pluginViews[i]->transform.setCenterByOffsets(Vector2f(0, i*120), Vector2f(0, 0));
	}
}

const Material* PluginManagerView::getMaterial() const
{
	return material;
}

void PluginManagerView::renderImmediate(Renderer* target)
{
}
