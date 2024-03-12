#include "PluginView.hpp"

#include "gui/HUD.hpp"
#include "gui/LabelWidget.hpp"
#include "Resources.hpp"
#include "application/Plugin.hpp"

void PluginView::tryInit()
{
	/*
	   o--------------o--------o
	   |     name     | status |  50px
	   o--------------o--------o
	   |         path          |  50px
	   o-----------------------o
	*/

	if (!name)
	{
		name = hud->addWidget<LabelWidget>(Resources::textMat, Resources::headerFont);
		name->transform.setParent(&transform);
		name->transform.snapToCorner(Vector2f(0, 0), Vector2f(0, 50));
		name->transform.fillParentX();
	}

	if (!status)
	{
		status = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		status->align = UIAnchor({ 1, 0.5f }); //Status should be right-aligned
		status->transform.setParent(&transform);
		status->transform.snapToCorner(Vector2f(1, 0), Vector2f(0, 50));
		status->transform.fillParentX();
	}

	if (!path)
	{
		path = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		path->transform.setParent(&transform);
		path->transform.snapToCorner(Vector2f(0, 0), Vector2f(0, 50));
		path->transform.fillParentX();
		path->transform.setCenterByOffsets(Vector2f(0, 50), Vector2f(0, 0));
	}
}

PluginView::PluginView(HUD* hud) :
	Widget(hud),
	plugin(nullptr),
	path(nullptr),
	name(nullptr),
	status(nullptr)
{	
}

PluginView::~PluginView()
{
}

void PluginView::setViewed(Plugin* plugin)
{
	this->plugin = plugin;
	tryInit();
}

void PluginView::tick()
{
	if (!plugin) return;

	std::filesystem::path relPath = std::filesystem::relative(plugin->getPath());
	path->setText(relPath.wstring());

	name->setText(plugin->reportedData ? plugin->reportedData->name : L"<unloaded>");

	switch (plugin->status)
	{
		case Plugin::Status::NotLoaded   : status->setText(L"Not loaded"); break;
		case Plugin::Status::DllLoaded   : status->setText(L"DLL loaded"); break;
		case Plugin::Status::Registered  : status->setText(L"Registered"); break;
		case Plugin::Status::Hooked      : status->setText(L"Hooked"    ); break;

		default: assert(false); break;
	}
}

const Material* PluginView::getMaterial() const
{
	return nullptr;
}

void PluginView::renderImmediate(Renderer* target)
{
}
