#include "game/gui/Widget.hpp"

#include "Material.hpp"

Widget::Widget()
{
}

Widget::~Widget()
{
}

const ShaderProgram* Widget::getShader() const
{
    const Material* material = getMaterial();
    return material ? material->getShader() : nullptr;
}
