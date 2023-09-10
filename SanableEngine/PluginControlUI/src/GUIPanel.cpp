#include "GUIPanel.hpp"

#include <GraphicsSystem.h>

GUI::Panel::Panel(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const Color& backgroundColor, GraphicsBufferManager* graphicsBufferManager) :
	Container(theID, relativePosition, size),
	backgroundColor(backgroundColor)
{
	internalCanvasOwner = graphicsBufferManager;
	internalCanvasID = makeGraphicsID(theID, "internalCanvas");
	internalCanvas = internalCanvasOwner->createGraphicsBuffer(internalCanvasID, (int)size.getX(), (int)size.getY());
}

GUI::Panel::Panel(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, const Color& backgroundColor, GraphicsBufferManager* graphicsBufferManager, std::initializer_list<Element*> children) :
	Container(theID, relativePosition, size, children),
	backgroundColor(backgroundColor)
{
	internalCanvasOwner = graphicsBufferManager;
	internalCanvasID = makeGraphicsID(theID, "internalCanvas");
	internalCanvas = internalCanvasOwner->createGraphicsBuffer(internalCanvasID, (int)size.getX(), (int)size.getY());
}

GUI::Panel::~Panel()
{
	internalCanvasOwner->deleteBuffer(internalCanvasID);
}

void GUI::Panel::draw(GraphicsBuffer* dest)
{
	if (isActive())
	{
		//Redraw internal canvas only if marked dirty
		if (isDirty()) {
			std::cout << "Panel " << mID << " was dirty, redrawing" << std::endl;

			//Draw background
			GraphicsSystem::renderRect(*internalCanvas, ZERO_Vector3<float>, internalCanvas->getWidth(), internalCanvas->getHeight(), backgroundColor, true);

			//Draw children onto internal canvas
			Container::draw(internalCanvas);

			//We just drew. Mark everything as not dirty.
			clearDirty();
		}
		
		//Draw internal canvas onto parent
		GraphicsSystem::draw(*dest, cachedRenderPosition, *internalCanvas);
	}
}

void GUI::Panel::rebuildAbsoluteTransform(const Vector3<float>& parentAbsPos)
{
	Element::rebuildAbsoluteTransform(parentAbsPos);
	rebuildChildAbsoluteTransforms();
}

void GUI::Panel::rebuildRenderTransform(const Vector3<float>& rootRenderPos)
{
	cachedRenderPosition = ZERO_Vector3<float>; //FIXME kludge
	rebuildChildRenderTransforms();
	Element::rebuildRenderTransform(rootRenderPos);
}

bool GUI::Panel::isDirty() const
{
	bool dirty = Element::isDirty();
	for (Element* i : children) dirty |= i->isDirty(); //TODO short-circuit?
	return dirty;
}

void GUI::Panel::clearDirty()
{
	Element::clearDirty();
	for (Element* i : children) i->clearDirty();
}
