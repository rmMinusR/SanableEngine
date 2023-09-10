#pragma once

#include <vector>

#include "GraphicsBuffer.h"
#include "GraphicsBufferManager.h"

#include "GUIElement.hpp"

namespace GUI
{

	//A Container is an Element that can own other elements, distributing draw() and tick(). If moved, children will move with it.

	class Container : public Element
	{
	public:
		Container(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size);
		Container(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size, std::initializer_list<Element*> children);
		virtual ~Container();

		void addChild(Element* child); //NOTE: Container takes ownership of children
		inline void addChildren(std::initializer_list<Element*> children)  { for (Element* i : children) addChild(i); } //Syntax sugar
		void removeAllChildren();

		virtual void tick(System* system) override;
		virtual void draw(GraphicsBuffer* dest) override;

		virtual void rebuildAbsoluteTransform(const Vector3<float>& parentAbsPos) override;
		virtual void rebuildRenderTransform  (const Vector3<float>& rootRenderPos) override;
		virtual void rebuildChildAbsoluteTransforms();
		virtual void rebuildChildRenderTransforms();

		virtual void visit(const std::function<void(Element*)>& visitor) override;

	protected:
		std::vector<Element*> children;
	};

}