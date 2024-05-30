#include "gui/ImageWidget.hpp"

#include <GL/glew.h>
#include "Renderer.hpp"
#include "gui/UISprite.hpp"

ImageWidget::ImageWidget(HUD* hud, Material* material, const UISprite* sprite) :
	Widget(hud),
	material(material),
	sprite(sprite)
{
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::setSprite(const UISprite* newSprite)
{
	sprite = newSprite;
}

const Material* ImageWidget::getMaterial() const
{
	return material;
}

void ImageWidget::renderImmediate(Renderer* renderer)
{
	Rect<float> r = getTransform()->getRect();
	auto depth = getTransform()->getRenderDepth();
	
	renderer->errorCheck();
	//No need to send position: already applied via model matrix
	sprite->renderImmediate(renderer, Vector3f(0, 0, depth), r.size.x, r.size.y);
	renderer->errorCheck();
}
