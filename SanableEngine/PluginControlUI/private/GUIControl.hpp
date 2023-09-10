#pragma once

#include "EventListener.h"
#include "Vector3.inl"
#include "Color.hpp"

#include "GUIElement.hpp"

namespace GUI
{

	enum class ControlState
	{
		NORMAL,
		HOVERED,
		PRESSED
	};

	struct ControlColorProfile
	{
		Color normal;
		Color hovered;
		Color pressed;
	};

	//A Control is an abstraction for anything that can be interacted with by mouse. It also handles event registration.

	class Control : public Element, public EventListener
	{
	public:
		Control(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const ControlColorProfile& colorProfile);
		virtual ~Control();

		virtual void handleEvent(const Event& theEvent) override = 0;

	private:
		ControlState currentState;

	protected:
		ControlColorProfile colorProfile;

		inline const ControlState& getCurrentState() { return currentState; }
		inline void setCurrentState(const ControlState& val) { currentState = val; markDirty(); }

		inline const Color& getCurrentColor() const //Syntax sugar
		{
			return currentState == ControlState::NORMAL  ? colorProfile.normal  :
				   currentState == ControlState::HOVERED ? colorProfile.hovered :
				   colorProfile.pressed;
		}

		inline Color& getCurrentColor() //Syntax sugar
		{
			return currentState == ControlState::NORMAL  ? colorProfile.normal  :
				   currentState == ControlState::HOVERED ? colorProfile.hovered :
				   colorProfile.pressed;
		}
	};

}