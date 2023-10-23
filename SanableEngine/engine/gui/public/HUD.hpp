#include "GUIRoot.hpp"

#include "GUIContainer.hpp"

class HUD : public GUIRoot, private GUIContainer
{
	SANABLE_REFLECTION_HOOKS

public:
	ENGINEGUI_API HUD();
	ENGINEGUI_API virtual ~HUD();

	//Called by Renderer
	virtual void draw(Renderer* renderer, Rect<float> workingArea) const override;

	virtual void visit(const std::function<GUIElement*>& visitor, bool recurse) override;

	ENGINEGUI_API HUD(HUD&& mov);
	ENGINEGUI_API HUD& operator=(HUD&& mov);
	HUD(const HUD& cpy) = delete;
	HUD& operator=(const HUD& cpy) = delete;
};
