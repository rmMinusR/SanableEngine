#include "gui/Widget.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Material.hpp"
#include "Renderer.hpp"
#include "gui/HUD.hpp"

bool Widget::raycastExact(Vector2f pos) const
{
    return transform->getRect().contains(pos);
}

Widget::Widget(HUD* hud) :
    hud(hud)
{
    transform = hud->getMemory()->create<WidgetTransform>(this);
}

Widget::~Widget()
{
    hud->getMemory()->destroy(transform);
}

void Widget::tick()
{
}

WidgetTransform* Widget::getTransform()
{
    return transform;
}

const WidgetTransform* Widget::getTransform() const
{
    return transform;
}

void Widget::onMouseDown(Vector2f pos)
{
}

void Widget::onMouseUp(Vector2f pos)
{
}

void Widget::onMouseExit(Vector2f pos)
{
}

void Widget::onMouseEnter(Vector2f pos)
{
}

void Widget::onClicked(Vector2f pos)
{
}

void Widget::onDragStarted(Vector2f dragStartPos, Vector2f currentMousePos)
{
}

void Widget::whileDragged(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f currentMousePos)
{
}

void Widget::onDragFinished(Vector2f dragStartPos, Widget* dragStartWidget, Vector2f dragEndPos, Widget* dragEndWidget)
{
}

const ShaderProgram* Widget::getShader() const
{
    const Material* material = getMaterial();
    return material ? material->getShader() : nullptr;
}

void Widget::loadModelTransform(Renderer* renderer) const
{
    renderer->loadTransform(*transform);
}
