#include "gui/TabView.hpp"

#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/ButtonWidget.hpp"
#include "gui/HorizontalGroupWidget.hpp"
#include "gui/VerticalGroupWidget.hpp"
#include "gui/DraggableTabButton.hpp"

TabView::TabView(HUD* hud, Vector2f tabBtnSize, const Material* tabBtnBgMat, const RadioButtonWidget::SpriteSet tabSprites, TabsLocation tabsLocation, bool reorderable) :
	Widget(hud),
	reorderable(reorderable)
{
	// TODO WidgetTransform::setChildIndex()
	contentTransform = hud->getMemory()->create<WidgetTransform>(nullptr, hud);
	contentTransform->setParent(this->getTransform());
	AnchoredPositioning* contentPositioner = contentTransform->setPositioningStrategy<AnchoredPositioning>();
	contentPositioner->fillParent();

	// Lay out tabs/content panel
	if (tabsLocation == TabsLocation::Left)
	{
		tabArea = hud->addWidget<RadioButtonGroup>(
			tabBtnBgMat,
			tabSprites,
			LayoutUtil::LinearElementView().setFixedSize(tabBtnSize.y),
			true
		);
		AnchoredPositioning* tabAreaPositioner = tabArea->getTransform()->setPositioningStrategy<AnchoredPositioning>();
		tabAreaPositioner->minCorner = UIAnchor({ 0,0 }, { 0,0 });
		tabAreaPositioner->maxCorner = UIAnchor({ 0,1 }, { tabBtnSize.x,0 });
		contentPositioner->minCorner.offset.x += tabBtnSize.x;
	}
	else if (tabsLocation == TabsLocation::Right)
	{
		tabArea = hud->addWidget<RadioButtonGroup>(
			tabBtnBgMat,
			tabSprites,
			LayoutUtil::LinearElementView().setFixedSize(tabBtnSize.y),
			true
		);
		AnchoredPositioning* tabAreaPositioner = tabArea->getTransform()->setPositioningStrategy<AnchoredPositioning>();
		tabAreaPositioner->minCorner = UIAnchor({ 1,0 }, { -tabBtnSize.x,0 });
		tabAreaPositioner->maxCorner = UIAnchor({ 1,1 }, { 0,0 });
		contentPositioner->maxCorner.offset.x += -tabBtnSize.x;
	}
	else if (tabsLocation == TabsLocation::Top)
	{
		tabArea = hud->addWidget<RadioButtonGroup>(
			tabBtnBgMat,
			tabSprites,
			LayoutUtil::LinearElementView().setFixedSize(tabBtnSize.x),
			false
		);
		AnchoredPositioning* tabAreaPositioner = tabArea->getTransform()->setPositioningStrategy<AnchoredPositioning>();
		tabAreaPositioner->minCorner = UIAnchor({ 0,0 }, { 0,0 });
		tabAreaPositioner->maxCorner = UIAnchor({ 1,0 }, { 0,tabBtnSize.y });
		contentPositioner->minCorner.offset.y += tabBtnSize.y;
	}
	else if (tabsLocation == TabsLocation::Bottom)
	{
		tabArea = hud->addWidget<RadioButtonGroup>(
			tabBtnBgMat,
			tabSprites,
			LayoutUtil::LinearElementView().setFixedSize(tabBtnSize.x),
			false
		);
		AnchoredPositioning* tabAreaPositioner = tabArea->getTransform()->setPositioningStrategy<AnchoredPositioning>();
		tabAreaPositioner->minCorner = UIAnchor({ 0,1 }, { 0,-tabBtnSize.y });
		tabAreaPositioner->maxCorner = UIAnchor({ 1,1 }, { 0,0 });
		contentPositioner->maxCorner.offset += -tabBtnSize.y;
	}
	else assert(false);

	tabArea->getTransform()->setParent(this->getTransform());
	tabArea->setCallback([this](size_t from, size_t to) { this->tabSelectCallback(from, to); });
}

TabView::~TabView()
{
	hud->getMemory()->destroy(contentTransform);
}

RadioButtonWidget* TabView::addItem(Widget* widget)
{
	// Set up tab button
	//RadioButtonWidget* tabBtn = tabArea->addItem();
	RadioButtonWidget* tabBtn;
	if (reorderable)
	{
		tabBtn = hud->addWidget<DraggableTabButton>(widget, tabArea, tabArea->getSprites(), tabArea->getButtonMaterial());
		tabArea->addItem(tabBtn);
	}
	else tabBtn = tabArea->addItem(); // Non-reorderable

	// Set up tab content
	widget->getTransform()->setParent(contentTransform);
	widget->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();
	widget->getTransform()->setVisibility(contentTransform->getChildrenCount() > 1 ? WidgetVisibility::Collapsed : WidgetVisibility::Visible);

	return tabBtn;
}

void TabView::select(size_t newSelectionIndex)
{
	tabArea->select(newSelectionIndex);
}

void TabView::tabSelectCallback(size_t from, size_t to)
{
	contentTransform->getChild(from)->setVisibility(WidgetVisibility::Collapsed);
	contentTransform->getChild(to)->setVisibility(WidgetVisibility::Visible);
}

const Material* TabView::getMaterial() const
{
	return nullptr;
}

void TabView::renderImmediate(Renderer* renderer)
{
}

RadioButtonGroup* TabView::getTabArea()
{
	return tabArea;
}

WidgetTransform* TabView::getContentTransform()
{
	return contentTransform;
}
