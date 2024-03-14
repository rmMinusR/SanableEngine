#include "gui/ImageWidget.hpp"

#include <GL/glew.h>
#include "Renderer.hpp"
#include "Texture.hpp"

ImageWidget::ImageWidget(HUD* hud, Material* material, GTexture* texture) :
	Widget(hud),
	material(material),
	texture(texture)
{
}

ImageWidget::~ImageWidget()
{
}

const Material* ImageWidget::getMaterial() const
{
	return material;
}

void ImageWidget::renderImmediate(Renderer* renderer)
{
	Rect<float> r = transform.getRect();
	auto depth = transform.getRenderDepth();
	
	renderer->errorCheck();
	//No need to send position: already applied via model matrix
	renderer->drawTexture(texture, Vector3f(0, 0, depth), r.size.x, r.size.y);
	renderer->errorCheck();
}
