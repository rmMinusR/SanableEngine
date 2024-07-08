#pragma once

#include <ReflectionSpec.hpp>
#include "WidgetTransform.hpp"

class Renderer;
class HUD;
class Material;
class ShaderProgram;

class STIX_ENABLE_IMAGE_CAPTURE Widget
{
private:
	WidgetTransform* transform;

protected:
	HUD* hud;
	friend class HUD;
	ENGINEGUI_API virtual bool raycastExact(Vector2f pos) const;

public:
	ENGINEGUI_API Widget(HUD* hud);
	ENGINEGUI_API virtual ~Widget();

	ENGINEGUI_API virtual void tick();

	ENGINEGUI_API WidgetTransform* getTransform();
	ENGINEGUI_API const WidgetTransform* getTransform() const;
	ENGINEGUI_API HUD* getHUD() const;

	//Executes only on closest widget clicked/moused over
	ENGINEGUI_API virtual void onMouseDown(Vector2f pos);
	ENGINEGUI_API virtual void onMouseUp(Vector2f pos);
	ENGINEGUI_API virtual void onMouseExit(Vector2f pos);
	ENGINEGUI_API virtual void onMouseEnter(Vector2f pos);
	ENGINEGUI_API virtual void onClicked(Vector2f pos);

	ENGINEGUI_API virtual void onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos);
	ENGINEGUI_API virtual void whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos, Widget* currentlyHoveredWidget); //Executes both on start widget and currently hovered widget
	ENGINEGUI_API virtual void onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget); //Executes both on start widget and end widget

	ENGINEGUI_API const ShaderProgram* getShader() const;
	virtual const Material* getMaterial() const = 0;
	ENGINEGUI_API virtual void loadModelTransform(Renderer* renderer) const; //Makes no assumptions about current model state or model mode, just writes
	virtual void renderImmediate(Renderer* renderer) = 0;
};
