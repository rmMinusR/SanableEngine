#include "gui/Widget.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Material.hpp"
#include "Renderer.hpp"

bool Widget::raycastExact(Vector2f pos) const
{
    return transform.getRect().contains(pos);
}

Widget::Widget(HUD* hud) :
    hud(hud)
{
}

Widget::~Widget()
{
}

void Widget::refreshLayout()
{
}

void Widget::tick()
{
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

const ShaderProgram* Widget::getShader() const
{
    const Material* material = getMaterial();
    return material ? material->getShader() : nullptr;
}

void Widget::loadModelTransform(Renderer* renderer) const
{
    renderer->loadTransform(transform);
}
