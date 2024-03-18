#include "gui/HUD.hpp"

#include <unordered_map>
#include <algorithm>

#include "ShaderProgram.hpp"
#include "Material.hpp"

void HUD::applyConcurrencyBuffers()
{
	for (Widget* w : addQueue) widgets.add(w);
	addQueue.clear();

	for (Widget* w : removeQueue) widgets.remove(w);
	removeQueue.clear();
}

void HUD::addWidget_internal(Widget* widget)
{
	addQueue.push_back(widget);
	if (!widget->transform.getParent()) widget->transform.setParent(getRootTransform());
}

void HUD::removeWidget_internal(Widget* widget)
{
	removeQueue.push_back(widget);
}

HUD::HUD()
{
	root.setMinCornerRatio({ 0, 0 });
	root.setMaxCornerRatio({ 0, 0 });
}

HUD::~HUD()
{
}

MemoryManager* HUD::getMemory()
{
	return &memory;
}

void HUD::refreshLayout(Rect<float> viewport)
{
	root.setRectByOffsets(viewport);

	applyConcurrencyBuffers();
	widgets.memberCall(&Widget::refreshLayout);
	applyConcurrencyBuffers();

	//TODO transform caching goes here
}

void HUD::tick()
{
	applyConcurrencyBuffers();
	widgets.memberCall(&Widget::tick);
	applyConcurrencyBuffers();
}

void HUD::render(Rect<float> viewport, Renderer* renderer)
{
	root.setRectByOffsets(viewport);

	applyConcurrencyBuffers();
	
	//Collect objects to buffer
	std::unordered_map<
		const ShaderProgram*, //Group by shader
		std::unordered_map<
			const Material*, //Then by material
			std::vector<Widget*>
		>
	> renderables; //Note: No need for a CallBatcher here, we're guaranteed widgets will be grouped by type since our data source is a CallBatcher
	widgets.staticCall([&](Widget* w)
	{
		renderables[w->getShader()][w->getMaterial()].push_back(w);
	});

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//Process buffer
	for (const auto& shaderGroup : renderables)
	{
		//Activate shader
		if (shaderGroup.first) shaderGroup.first->activate();
		else ShaderProgram::clear();

		for (const auto& materialGroup : shaderGroup.second)
		{
			//Activate material
			if (materialGroup.first) materialGroup.first->writeSharedUniforms(renderer);
			assert(materialGroup.first == nullptr || materialGroup.first->getShader() == shaderGroup.first);

			for (Widget* w : materialGroup.second)
			{
				w->loadModelTransform(renderer);

				if (materialGroup.first) materialGroup.first->writeInstanceUniforms(renderer, w);

				assert(w->getMaterial() == materialGroup.first);
				assert(w->getMaterial() == nullptr || w->getMaterial()->getShader() == shaderGroup.first);
				w->renderImmediate(renderer);
			}
		}
	}

	glPopMatrix();
}

void HUD::raycast(Vector2f pos, const std::function<void(Widget*)>& visitor) const
{
	std::vector<Widget*> hits;
	widgets.staticCall([&](Widget* w) { if (w->transform.getRect().contains(pos)) hits.push_back(w); }); //FIXME inefficient as heck, especially without cached transforms
	std::sort(hits.begin(), hits.end(), [](Widget* a, Widget* b) { return a->transform.getRenderDepth() > b->transform.getRenderDepth(); });
	for (Widget* w : hits) visitor(w);
}

WidgetTransform const* HUD::getRootTransform() const
{
	return &root;
}
