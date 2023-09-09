#pragma once

#include "GUIControl.hpp"

namespace GUI {

	//A Slider is a control that can be clicked and dragged, sending a callback whenever its value changes. Its range is definable, and it supports both logarithmic and linear (aka logBase=1) scales.

	class Slider : public Control
	{
	private:
		constexpr static float channelSize = 10; //Absolute units
		constexpr static float knobSize = 20; //Absolute units

	public:
		typedef std::function<void(const float&)> update_callback_t;

		Slider(ControlID theID, const Vector2D& relativePosition, const Vector2D& size, float fillValue, float lowerBound, float upperBound,                const ControlColorProfile& knobColorProfile, const Color& channelColor, const update_callback_t& onUpdate);
		Slider(ControlID theID, const Vector2D& relativePosition, const Vector2D& size, float fillValue, float lowerBound, float upperBound, float mBase, const ControlColorProfile& knobColorProfile, const Color& channelColor, const update_callback_t& onUpdate);

		void draw(GraphicsBuffer* dest) override;

		void handleEvent(const Event& theEvent) override;

		const float& getFillValue() const;
		void setFillValue(const float& val, bool sendEvent = true, bool doClamp = true);

		const float& getFillRatio() const;
		void setFillRatio(const float& amt, bool sendEvent = true, bool doClamp = true);

	protected:
		float fillRatio; //Always ranges 0-1

		float lowerBound; float logLowerBound;
		float upperBound; float logUpperBound;
		void calcLogBounds();
		float mBase; //1 = no logarithms applied
		bool isLogarithmic() const;
		float fillValue;
		void calcFillValue();

		Color channelColor;

		update_callback_t onUpdate;
	};

}