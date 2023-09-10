#pragma once

#include <functional>

#include "GUIControl.hpp"

namespace GUI {

	//A Checkbox is a Control that can be toggled on or off with a click, sending a callback whenever this occurs.

	class Checkbox : public Control
	{
	public:
		typedef std::function<void(const bool&)> callback_func_t;

		Checkbox(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const bool& isChecked, const float& checkSizeRatio, const ControlColorProfile& colorProfile, const Color& checkColor, callback_func_t&& callback);
		
		void draw(GraphicsBuffer* dest) override;

		void handleEvent(const Event& theEvent) override;

	private:
		bool isChecked;
		float checkSizeRatio;
		Color checkColor;

		callback_func_t callback;
	};

}

