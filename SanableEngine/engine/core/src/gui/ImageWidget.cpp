#include "gui/ImageWidget.hpp"

#include <GL/glew.h>
#include "Renderer.hpp"
#include "Texture.hpp"

ImageWidget::ImageWidget(Material* material, Texture* texture) :
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
	
	renderer->drawTexture(*texture, Vector3f(r.x, r.y, depth), r.width, r.height);
}
