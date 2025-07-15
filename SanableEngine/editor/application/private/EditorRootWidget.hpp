#pragma once

#include "gui/Widget.hpp"

class HierarchyViewPane;
class SceneViewPane;
class PlayInEditor;

class EditorRootWidget : public Widget
{
private:
	HierarchyViewPane* hierarchy;
	SceneViewPane* scene;
	PlayInEditor* player;

public:
	EditorRootWidget(HUD* hud);
	virtual ~EditorRootWidget();
};
