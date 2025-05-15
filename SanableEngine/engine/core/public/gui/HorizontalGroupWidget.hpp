#pragma once

#include "LinearLayoutGroupWidget.hpp"

class HorizontalGroupWidget : public LinearLayoutGroupWidget
{
public:
	ENGINEGUI_API HorizontalGroupWidget(HUD* hud);
	ENGINEGUI_API virtual ~HorizontalGroupWidget();

protected:
	ENGINEGUI_API virtual void refreshLayout() override;
};
