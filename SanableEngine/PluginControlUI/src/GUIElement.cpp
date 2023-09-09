#include "GUIElement.hpp"

#include <sstream>

ControlID nextID = 0;
ControlID GUI::generateID()
{
	return nextID++;
}

GraphicsBufferID GUI::makeGraphicsID(const ControlID& id, const std::string& useCase)
{
	std::ostringstream sout;
	sout << "GUIControl-" << id << ":" << useCase;
	return GraphicsBufferID(sout.str());
}

GUI::Element::Element(const ControlID& theID, const Vector2D& relativePosition, const Vector2D& size) :
	mID(theID),
	relativePosition(relativePosition),
	size(size),
	mIsActive(true),
	parent(nullptr),
	mIsDirty(true)
{
}

void GUI::Element::tick(System* system)
{
	//Nothing to do here, but children will override
}

void GUI::Element::rebuildAbsoluteTransform(const Vector2D& parentAbsPos)
{
	cachedAbsolutePosition = parentAbsPos;
	cachedAbsolutePosition += relativePosition;
}

void GUI::Element::rebuildRenderTransform(const Vector2D& rootRenderPos)
{
	cachedRenderPosition = rootRenderPos;
	cachedRenderPosition += relativePosition;
}

bool GUI::Element::intersects(const Vector2D& pos) const
{
	Vector2D relPos = pos - cachedAbsolutePosition;
	return 0 < relPos.getX() && relPos.getX() < size.getX()
		&& 0 < relPos.getY() && relPos.getY() < size.getY();
}

void GUI::Element::setActive(const bool& active)
{
	mIsActive = active;
}

bool GUI::Element::isActiveInHierarchy() const
{
	return isActive() && (!parent || parent->isActiveInHierarchy());
}

void GUI::Element::visit(const std::function<void(Element*)>& visitor)
{
	visitor(this);
}