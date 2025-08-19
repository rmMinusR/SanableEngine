#include "WidgetAsWindow_RenderProxy.hpp"

#include "WidgetAsWindow.hpp"

WidgetAsWindow_RenderProxy::WidgetAsWindow_RenderProxy(Renderer* implementation, WidgetAsWindow* proxyOwner) :
	Renderer(proxyOwner),
	implementation(implementation)
{
}

WidgetAsWindow_RenderProxy::~WidgetAsWindow_RenderProxy()
{
}

void WidgetAsWindow_RenderProxy::setDefaultFramebuffer(Framebuffer* framebuffer)
{
	defaultFramebuffer = framebuffer;
}

void WidgetAsWindow_RenderProxy::activate() const
{
	implementation->activate();
}

void WidgetAsWindow_RenderProxy::drawRect(Vector3f center, float w, float h, const Color4<uint8_t>& color)
{
	implementation->drawRect(center, w, h, color);
}

void WidgetAsWindow_RenderProxy::drawTextNonShadered(const Font& font, const std::wstring& text, Vector3f pos)
{
	implementation->drawTextNonShadered(font, text, pos);
}

void WidgetAsWindow_RenderProxy::drawText(const Font& font, const Material& mat, const std::wstring& text, const Color4<uint8_t>& color)
{
	implementation->drawText(font, mat, text, color);
}

void WidgetAsWindow_RenderProxy::drawTexture(const GTexture* tex, const Material* mat, Vector3f pos, float w, float h)
{
	implementation->drawTexture(tex, mat, pos, w, h);
}

void WidgetAsWindow_RenderProxy::drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h)
{
	implementation->drawSprite(spr, mat, pos, w, h);
}

void WidgetAsWindow_RenderProxy::drawSprite(const Sprite* spr, const Material* mat, Vector3f pos, float w, float h, Color4<uint8_t> tintColor)
{
	implementation->drawSprite(spr, mat, pos, w, h, tintColor);
}

void WidgetAsWindow_RenderProxy::drawCMeshImmediate(const CMesh* mesh)
{
	implementation->drawCMeshImmediate(mesh);
}

void WidgetAsWindow_RenderProxy::setViewProjTranform(const glm::mat4& mat)
{
	implementation->setViewProjTranform(mat);
}

void WidgetAsWindow_RenderProxy::setModelTransform(const glm::mat4& mat)
{
	implementation->setModelTransform(mat);
}

glm::mat4 WidgetAsWindow_RenderProxy::getViewProjTranform() const
{
	return implementation->getViewProjTranform();
}

glm::mat4 WidgetAsWindow_RenderProxy::getModelTransform() const
{
	return implementation->getModelTransform();
}

ShaderUniform::GlobalData WidgetAsWindow_RenderProxy::getCurGlobalData() const
{
	return implementation->getCurGlobalData();
}

void WidgetAsWindow_RenderProxy::setActiveShader(const ShaderProgram* source)
{
	implementation->setActiveShader(source);
}

void WidgetAsWindow_RenderProxy::setMaterialFlags(const Material& material)
{
	implementation->setMaterialFlags(material);
}

void WidgetAsWindow_RenderProxy::beginFrame(const Camera& camSettings, Rect<float> viewport, Vector3<float> position, glm::quat rotation, const Framebuffer* framebuffer)
{
	implementation->beginFrame(camSettings, viewport, position, rotation, framebuffer ? framebuffer : defaultFramebuffer);
}

void WidgetAsWindow_RenderProxy::endFrame()
{
	implementation->endFrame();
}

void WidgetAsWindow_RenderProxy::clear(Color4<float> color)
{
	implementation->clear(color);
}

GTexture* WidgetAsWindow_RenderProxy::loadTexture(const std::filesystem::path& path)
{
	return implementation->loadTexture(path);
}

GTexture* WidgetAsWindow_RenderProxy::newTexture(int width, int height, int nChannels, void* data)
{
	return implementation->newTexture(width, height, nChannels, data);
}

GMesh* WidgetAsWindow_RenderProxy::newMesh(const CMesh& source)
{
	return implementation->newMesh(source);
}

ShaderProgram* WidgetAsWindow_RenderProxy::loadShaderProgram(const std::filesystem::path& path)
{
	return implementation->loadShaderProgram(path);
}

Framebuffer* WidgetAsWindow_RenderProxy::newFramebuffer(Vector2<int> initialSize, const Framebuffer::Settings& settings)
{
	return implementation->newFramebuffer(initialSize, settings);
}

void WidgetAsWindow_RenderProxy::errorCheck() const
{
	implementation->errorCheck();
}
