#include "gui/ImageWidget.hpp"

#include <GL/glew.h>
#include "Renderer.hpp"
#include "gui/UISprite.hpp"

ImageWidget::ImageWidget(HUD* hud, Material* material, const UISprite* sprite) :
	Widget(hud),
	material(material),
	sprite(sprite)
{
	tintColor = { 255, 255, 255, 255 };
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::setSprite(const UISprite* newSprite)
{
	sprite = newSprite;
}

void ImageWidget::setTintColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	tintColor = { r, g, b, a };
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
	sprite->renderImmediate(renderer, material, Vector3f(0, 0, 0), r.size, tintColor);
	renderer->errorCheck();
}
