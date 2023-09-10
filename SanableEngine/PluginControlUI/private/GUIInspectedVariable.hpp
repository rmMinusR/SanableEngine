#pragma once

#include "GUILiveText.hpp"

namespace GUI
{

	//An InspectedVariable shows a variable's value as it changes. Based on Unity's inspector.

	template<typename T>
	class InspectedVariable : public LiveText
	{
	private:
		std::string label;
		std::string units;
		T const * varPtr;

		void build(std::ostringstream& sout) { sout << label << ": " << *varPtr << units; }

	public:
		InspectedVariable(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const Color& color, Font* font, const Font::Alignment& alignment, const std::string& label, const T& varRef, const std::string& units) :
			LiveText(theID, relativePosition, size, color, font, alignment, nullptr),
			label(label),
			varPtr(&varRef),
			units(units)
		{
			builder = std::bind(&InspectedVariable<T>::build, this, std::placeholders::_1);
		}
	};

}

