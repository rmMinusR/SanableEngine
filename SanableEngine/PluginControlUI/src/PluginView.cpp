#include "PluginView.hpp"

#include "gui/HUD.hpp"
#include "gui/LabelWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/UISprite.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/VerticalGroupWidget.hpp"
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

		VerticalGroupWidget* inner = hud->addWidget<VerticalGroupWidget>();
		inner->getTransform()->setParent(this->getTransform());
		inner->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();

		name = hud->addWidget<LabelWidget>(Resources::textMat, Resources::headerFont);
		name->getTransform()->setParent(inner->getTransform());
		name->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(inner);

		path = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		path->getTransform()->setParent(inner->getTransform());
		path->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(inner);

		HorizontalGroupWidget* statusLine = hud->addWidget<HorizontalGroupWidget>();
		statusLine->getTransform()->setParent(inner->getTransform());
		statusLine->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(inner);

		status = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont);
		status->getTransform()->setParent(statusLine->getTransform());
		status->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(statusLine)->flexWeight = 4;


		ButtonWidget::SpriteSet buttonSprites = { Resources::buttonNormalSprite, Resources::buttonPressedSprite, Resources::buttonDisabledSprite };

		imgToggleLoadedBg = hud->addWidget<ImageWidget>(nullptr, Resources::buttonNormalSprite);
		lblToggleLoaded   = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont, SDL_Color{ 0, 0, 0, 255 });
		lblToggleLoaded->align = Vector2f(0.5f, 0.5f);
		btnToggleLoaded = hud->addWidget<ButtonWidget>(imgToggleLoadedBg, buttonSprites);
		btnToggleLoaded->getTransform()->setParent(statusLine->getTransform());
		btnToggleLoaded->getContentSocket()->put(lblToggleLoaded);
		btnToggleLoaded->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(statusLine)->flexWeight = 3;
		btnToggleLoaded->setCallback(
			[&]() {
				if (this->plugin->isCodeLoaded()) this->mgr->unload(this->plugin);
				else                              this->mgr->load(this->plugin);
			}
		);

		imgToggleHookedBg = hud->addWidget<ImageWidget>(nullptr, Resources::buttonNormalSprite);
		lblToggleHooked   = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont, SDL_Color{ 0, 0, 0, 255 });
		lblToggleHooked->align = Vector2f(0.5f, 0.5f);
		btnToggleHooked = hud->addWidget<ButtonWidget>(imgToggleHookedBg, buttonSprites);
		btnToggleHooked->getTransform()->setParent(statusLine->getTransform());
		btnToggleHooked->getContentSocket()->put(lblToggleHooked);
		btnToggleHooked->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(statusLine)->flexWeight = 3;
		btnToggleHooked->setCallback(
			[&]() {
				if (this->plugin->isHooked()) this->mgr->unhook(this->plugin);
				else                          this->mgr->hook(this->plugin);
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
	if (this->plugin != plugin) lastKnownStatus = (Plugin::Status)-1;
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

	if (lastKnownStatus != plugin->status)
	{
		lastKnownStatus = plugin->status;
		switch (plugin->status)
		{
			case Plugin::Status::NotLoaded : status->setText(L"Not loaded"); btnToggleLoaded->setState(UIState::Normal  ); lblToggleLoaded->setText(L"Load"        ); btnToggleHooked->setState(UIState::Disabled); lblToggleHooked->setText(L"Can't hook"); break;
			case Plugin::Status::DllLoaded : status->setText(L"DLL loaded"); btnToggleLoaded->setState(UIState::Normal  ); lblToggleLoaded->setText(L"Load"        ); btnToggleHooked->setState(UIState::Disabled); lblToggleHooked->setText(L"Can't hook"); break;
			case Plugin::Status::Registered: status->setText(L"Registered"); btnToggleLoaded->setState(UIState::Normal  ); lblToggleLoaded->setText(L"Unload"      ); btnToggleHooked->setState(UIState::Normal  ); lblToggleHooked->setText(L"Hook"      ); break;
			case Plugin::Status::Hooked    : status->setText(L"Hooked"    ); btnToggleLoaded->setState(UIState::Disabled); lblToggleLoaded->setText(L"Can't unload"); btnToggleHooked->setState(UIState::Normal  ); lblToggleHooked->setText(L"Unhook"    ); break;

			default: assert(false); break;
		}
	}
	
}

const Material* PluginView::getMaterial() const
{
	return nullptr;
}

void PluginView::renderImmediate(Renderer* target)
{
}
