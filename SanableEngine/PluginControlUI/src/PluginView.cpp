#include "PluginView.hpp"

#include <locale>
#include <codecvt>

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
#include "TypeLayoutView.hpp"
#include "application/Application.hpp"
#include "application/Window.hpp"
#include "gui/WindowGUIInputProcessor.hpp"
#include "gui/WindowGUIRenderPipeline.hpp"
#include "TypeLayoutView.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Font.hpp"

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

		imgToggleLoadedBg = hud->addWidget<ImageWidget>(Resources::imageMat, Resources::buttonNormalSprite);
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

		imgToggleHookedBg = hud->addWidget<ImageWidget>(Resources::imageMat, Resources::buttonNormalSprite);
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

		imgInspectTypesBg = hud->addWidget<ImageWidget>(Resources::imageMat, Resources::buttonNormalSprite);
		lblInspectTypes   = hud->addWidget<LabelWidget>(Resources::textMat, Resources::labelFont, SDL_Color{ 0, 0, 0, 255 });
		lblInspectTypes->align = Vector2f(0.5f, 0.5f);
		lblInspectTypes->setText(L"Inspect RTTI");
		btnInspectTypes = hud->addWidget<ButtonWidget>(imgInspectTypesBg, buttonSprites);
		btnInspectTypes->getContentSocket()->put(lblInspectTypes);
		btnInspectTypes->getTransform()->setParent(statusLine->getTransform());
		btnInspectTypes->getTransform()->setPositioningStrategy<AutoLayoutPositioning>(statusLine)->flexWeight = 2;
		btnInspectTypes->setCallback(
			[&]() {
				std::wstring wname = this->plugin->reportedData->name;
				std::string name; name.resize(wname.length(), '\0');
				std::wcstombs(name.data(), wname.data(), wname.length());

				const TypeInfo* initialInspectedType = &this->plugin->getRTTI()->getTypes()[0];

				WindowBuilder wb = hud->getApplication()->buildWindow(name, 36*8, 36*(int)std::ceil(initialInspectedType->layout.size/8.0f)); //TODO remove magic numbers
				WindowGUIRenderPipeline* renderPipeline = new WindowGUIRenderPipeline(hud->getApplication());
				wb.setRenderPipeline(renderPipeline);
				Window* window = wb.build();
				window->setRenderPipeline(renderPipeline);
				window->setInputProcessor(new WindowGUIInputProcessor(&renderPipeline->hud, 5));
				
				//FIXME use shared rendering context instead
				GTexture* rttiFieldTexture = window->getRenderer()->loadTexture("resources/ui/textures/field.png");
				UISprite3x3* rttiFieldSprite = new UISprite3x3(rttiFieldTexture);
				rttiFieldSprite->setPixel({ 1,1 }, { 7,6 });
				rttiFieldSprite->setPixel({ 2,2 }, { 8,8 });

				ShaderProgram* imageShader = new ShaderProgram("resources/ui/shaders/image");
				if (!imageShader->load()) assert(false);
				Material* imageMat = new Material(imageShader);

				ShaderProgram* textShader = new ShaderProgram("resources/ui/shaders/font");
				if (!textShader->load()) assert(false);
				Material* textMat = new Material(textShader);
				Font* fieldFont = new Font("resources/ui/fonts/arial.ttf", 12);
				
				//TODO add dropdown selector for types
				TypeInfoView* v = renderPipeline->hud.addWidget<TypeInfoView>(initialInspectedType, imageMat, rttiFieldSprite, rttiFieldSprite, textMat, fieldFont);
				v->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();
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
			case Plugin::Status::NotLoaded : status->setText(L"Not loaded"); lblToggleLoaded->setText(L"Load"        ); lblToggleHooked->setText(L"Can't hook"); break;
			case Plugin::Status::DllLoaded : status->setText(L"DLL loaded"); lblToggleLoaded->setText(L"Load"        ); lblToggleHooked->setText(L"Can't hook"); break;
			case Plugin::Status::Registered: status->setText(L"Registered"); lblToggleLoaded->setText(L"Unload"      ); lblToggleHooked->setText(L"Hook"      ); break;
			case Plugin::Status::Hooked    : status->setText(L"Hooked"    ); lblToggleLoaded->setText(L"Can't unload"); lblToggleHooked->setText(L"Unhook"    ); break;

			default: assert(false); break;
		}

		btnToggleLoaded->setState(plugin->status != Plugin::Status::Hooked ? UIState::Normal : UIState::Disabled);
		btnToggleHooked->setState(plugin->status >= Plugin::Status::Registered ? UIState::Normal : UIState::Disabled);
		btnInspectTypes->setState(plugin->status != Plugin::Status::NotLoaded ? UIState::Normal : UIState::Disabled);
	}
	
}

const Material* PluginView::getMaterial() const
{
	return nullptr;
}

void PluginView::renderImmediate(Renderer* target)
{
}
