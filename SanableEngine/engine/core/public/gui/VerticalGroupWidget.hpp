#pragma once

#include <ReflectionSpec.hpp>
#include "LinearLayoutGroupWidget.hpp"

class VerticalGroupWidget : public LinearLayoutGroupWidget
{
public:
	ENGINEGUI_API VerticalGroupWidget(HUD* hud);
	ENGINEGUI_API virtual ~VerticalGroupWidget();

	ENGINEGUI_API virtual void refreshLayout() override;
};
