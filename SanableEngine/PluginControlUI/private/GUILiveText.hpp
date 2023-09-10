#pragma once

#include <functional>
#include <sstream>

#include "GUIText.hpp"

namespace GUI {

	//LiveText is Text that updates every frame according to a callback.

	class LiveText : public Text
	{
	public:
		typedef std::function<void(std::ostringstream&)> builder_t;

		LiveText(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const Color& color, Font* font, const Font::Alignment& alignment, builder_t&& builder);

		void tick(System* system) override;

		bool isDirty() const override { return true; }

	protected:
		builder_t builder;

		void updateContent();
	};

}

