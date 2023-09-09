#pragma once

#include <functional>

#include "GUIControl.hpp"
#include "GUIPanel.hpp"

namespace GUI
{

	//A Button is an Element that runs an arbitrary callback on press.

	class Button : public Control
	{
	public:
		typedef std::function<void()> callback_t;

	protected:
		callback_t onClicked;
		Panel* panel; //Both background color and labels
		
	public:
		Button(const ControlID& theID, const Vector2D& relativePosition, const Vector2D& size, const ControlColorProfile& colorProfile, GraphicsBufferManager* gbm, callback_t&& onClicked);
		Button(const ControlID& theID, const Vector2D& relativePosition, const Vector2D& size, const ControlColorProfile& colorProfile, GraphicsBufferManager* gbm, callback_t&& onClicked, std::initializer_list<Element*> labels);
		virtual ~Button();

		virtual void tick(System* system) override;
		virtual void draw(GraphicsBuffer* dest) override;
		
		virtual void rebuildAbsoluteTransform(const Vector2D& parentAbsPos ) override;
		virtual void rebuildRenderTransform  (const Vector2D& rootRenderPos) override;

		void handleEvent(const Event& theEvent) override;
		
		void setOnClicked(callback_t&& onClicked);
		
		virtual void visit(const std::function<void(Element*)>& visitor) override;
		
		inline void addLabel(Element* e) { panel->addChild(e); }
		inline void addLabels(std::initializer_list<Element*> els) { panel->addChildren(els); }
		inline void clearLabels() { panel->removeAllChildren(); }

		bool isDirty() const override;
		void clearDirty() override;
	};

}