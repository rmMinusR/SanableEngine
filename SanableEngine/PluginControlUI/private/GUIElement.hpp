#pragma once

#include <functional>

#include "Trackable.h"
#include "Defines.h"
#include "Vector3<float>.h"

#include <System.h>

class GraphicsBuffer;

typedef IDType ControlID;

/*

Element (base class, positional data and virtual functions etc)
 o Text
     o LiveText (calls a function every frame to update its content)
         o InspectedVariable (displays a variable's value, for debugging)
 o Control (color profile and event callback registration)
     o Button (callback on press)
     o Slider (callback whenever pressed or dragged)
     o Checkbox (callback on toggle)
 o Container (can hold other Elements, distributing tick() and draw())
     o Panel (renders child Elements to internal canvas first, enforcing bounds)

*/

namespace GUI
{

	ControlID generateID();

	GraphicsBufferID makeGraphicsID(const ControlID& id, const std::string& useCase);

	//An Element is the most basic abstraction of something that can be rendered to the GUI.

	class Element : public Trackable
	{
	public:
		Element(const ControlID& theID, const Vector3<float>& relativePosition, const Vector3<float>& size);
		virtual ~Element() {}

		virtual void tick(System* system);
		virtual void draw(GraphicsBuffer* dest) = 0;

		virtual void rebuildAbsoluteTransform(const Vector3<float>& parentAbsPos);
		virtual void rebuildRenderTransform  (const Vector3<float>& rootRenderPos);

		virtual bool intersects(const Vector3<float>& pos) const;

		void setActive(const bool& active);
		inline bool isActive() const { return mIsActive; }
		bool isActiveInHierarchy() const;

		//For use by Containers. Frontend programmer should never touch this.
		inline void setParent(Element* newParent) { parent = newParent; }
		inline Element* getParent() { return parent; }

		//Visitor pattern
		virtual void visit(const std::function<void(Element*)>& visitor);

	protected:
		ControlID mID;
		Element* parent;
		bool mIsActive; //Should we tick and render this object? (And its children if it's a Container)

		Vector3<float> relativePosition;
		
		//Internal render targets (like Container) require that we split these.
		Vector3<float> cachedAbsolutePosition; //Where is our parent, in absolute screen space? Mostly used in mouse event processing
		Vector3<float> cachedRenderPosition;   //Where should we render relative to? Mostly used in draw()

		Vector3<float> size;

	private: //Dirty draw system
		bool mIsDirty;
	public:
		virtual bool isDirty() const { return mIsDirty; } //Elements that want to render *every* frame should override this and return true. Containers and container-like objects should also poll their children.
		virtual void markDirty() { mIsDirty = true; }
		virtual void clearDirty() { mIsDirty = false; } //Containers and container-like objects should also reset their children 
	};

}