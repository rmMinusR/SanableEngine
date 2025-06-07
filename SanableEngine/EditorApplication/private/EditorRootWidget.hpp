#pragma once

#include "gui/Widget.hpp"

class EditorRootWidget : public Widget
{
	HierarchyViewPane* hierarchy;
	SceneViewPane* scene;
	PlayInEditor* player;
};
