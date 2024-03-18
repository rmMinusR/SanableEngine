#include "PluginView.hpp"

#include "gui/HUD.hpp"
#include "gui/LabelWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LayoutUtil.hpp"
#include "Resources.hpp"
#include "application/Plugin.hpp"
#include "application/PluginManager.hpp"

void PluginView::tryInit()
{
	/*
	              100%
	   o------------------------o
	   |          name          |  50px
	   o------------------------o
	   |          path          |  50px
	   o----------o------o------o
	   |  status  | load | hook |  50px
	   o----------o------o------o
	        40%      30%    30%
	*/

	if (!name)
	{
		assert(!path);
		assert(!status);
		assert(!btnToggleHooked);
		assert(!btnToggleLoaded);
		assert(!imgToggleLoadedBg);
		assert(!imgToggleHookedBg);
		assert(!lblToggleLoaded);
		assert(!lblToggleHooked);

		name = hud->addWidget<LabelWidget>(Resources::textMat, Resources::headerFont);
		name->transform.setParent(&transform);
		name->transform.snapToCorner(Vector2f(0, 0));

		path = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		path->transform.setParent(&transform);
		path->transform.snapToCorner(Vector2f(0, 0));

		status = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		status->transform.setParent(&transform);
		status->transform.snapToCorner(Vector2f(0, 0));

		LayoutUtil::Stretch::vertical(transform.getRect(),
			{
				{ &name  ->transform, 1 },
				{ &path  ->transform, 1 },
				{ &status->transform, 1 }
			}
		);
		name->transform.fillParentX();
		path->transform.fillParentX();

		imgToggleLoadedBg = hud->addWidget<ImageWidget>(nullptr, Resources::buttonBackground);
		lblToggleLoaded   = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		lblToggleLoaded->align = Vector2f(0.5f, 0.5f);
		btnToggleLoaded = hud->addWidget<ButtonWidget>(imgToggleLoadedBg, lblToggleLoaded);
		btnToggleLoaded->transform.setParent(&transform);
		btnToggleLoaded->transform.snapToCorner(Vector2f(0, 0));
		btnToggleLoaded->setCallback([&]() {
			if (!this->plugin->isHooked())
			{
				if (this->plugin->isCodeLoaded())
				{
					this->mgr->unload(this->plugin);
				}
				else
				{
					this->mgr->load(this->plugin);
				}
			}
		});

		imgToggleHookedBg = hud->addWidget<ImageWidget>(nullptr, Resources::buttonBackground);
		lblToggleHooked   = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		lblToggleHooked->align = Vector2f(0.5f, 0.5f);
		btnToggleHooked = hud->addWidget<ButtonWidget>(imgToggleHookedBg, lblToggleHooked);
		btnToggleHooked->transform.setParent(&transform);
		btnToggleHooked->transform.snapToCorner(Vector2f(0, 0));
		btnToggleHooked->setCallback([&]() {
			if (this->plugin->isCodeLoaded())
			{
				if (this->plugin->isHooked())
				{
					this->mgr->unhook(this->plugin);
				}
				else
				{
					this->mgr->hook(this->plugin);
				}
			}
		});

		status->transform.fillParentX();
		status->transform.setMaxCornerRatio(Vector2f(0, 0), true);
		LayoutUtil::Stretch::horizontal(
			status->transform.getRect(),
			{
				{ &status         ->transform, 40 },
				{ &btnToggleLoaded->transform, 30 },
				{ &btnToggleHooked->transform, 30 }
			}
		);
	}
}

PluginView::PluginView(HUD* hud) :
	Widget(hud),
	plugin(nullptr),
	path(nullptr),
	name(nullptr),
	status(nullptr),
	btnToggleLoaded(nullptr),
	btnToggleHooked(nullptr),
	imgToggleLoadedBg(nullptr),
	imgToggleHookedBg(nullptr),
	lblToggleLoaded(nullptr),
	lblToggleHooked(nullptr)
{	
}

PluginView::~PluginView()
{
}

void PluginView::setViewed(Plugin* plugin, PluginManager* mgr)
{
	this->plugin = plugin;
	this->mgr = mgr;
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
		case Plugin::Status::NotLoaded   : status->setText(L"Not loaded"); lblToggleLoaded->setText(L"Load"        ); lblToggleHooked->setText(L"Can't hook"); break;
		case Plugin::Status::DllLoaded   : status->setText(L"DLL loaded"); lblToggleLoaded->setText(L"Load"        ); lblToggleHooked->setText(L"Can't hook"); break;
		case Plugin::Status::Registered  : status->setText(L"Registered"); lblToggleLoaded->setText(L"Unload"      ); lblToggleHooked->setText(L"Hook"      ); break;
		case Plugin::Status::Hooked      : status->setText(L"Hooked"    ); lblToggleLoaded->setText(L"Can't unload"); lblToggleHooked->setText(L"Unhook"    ); break;

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
