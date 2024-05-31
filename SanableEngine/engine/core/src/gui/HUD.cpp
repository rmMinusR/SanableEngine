#include "gui/HUD.hpp"

#include <unordered_map>
#include <algorithm>

#include "ShaderProgram.hpp"
#include "Material.hpp"

void HUD::applyConcurrencyBuffers()
{
	//for (Widget* w : addQueue) widgets.add(w);
	addQueue.clear();

	//for (Widget* w : removeQueue) widgets.remove(w);
	for (Widget* w : removeQueue) memory.destroy(w);
	removeQueue.clear();
}

void HUD::addWidget_internal(Widget* widget)
{
	addQueue.push_back(widget);
	if (!widget->transform->getParent()) widget->transform->setParent(getRootTransform());
}

void HUD::removeWidget_internal(Widget* widget)
{
	removeQueue.push_back(widget);
}

HUD::HUD()
{
	root = memory.create<WidgetTransform>(nullptr, this);
	root->setPositioningStrategy_internal(nullptr);

	transforms = memory.getSpecificPool<WidgetTransform>(true);
}

HUD::~HUD()
{
	memory.destroy(root);
}

MemoryManager* HUD::getMemory()
{
	return &memory;
}

void HUD::refreshLayout(Rect<float> viewport)
{
	root->rect = root->localRect = viewport;

	applyConcurrencyBuffers();
	for (auto it = transforms->cbegin(); it != transforms->cend(); ++it) static_cast<WidgetTransform*>(*it)->refresh();
	applyConcurrencyBuffers();

	//TODO transform caching goes here
}

void HUD::tick()
{
	applyConcurrencyBuffers();
	memory.ensureFresh();
	widgets.ensureFresh(&memory);
	widgets.memberCall(&Widget::tick);
	applyConcurrencyBuffers();
}

void HUD::render(Renderer* renderer)
{
	applyConcurrencyBuffers();
	memory.ensureFresh();
	widgets.ensureFresh(&memory);
	
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

	/*
	// Debug: draw transform bounding boxes
	ShaderProgram::clear();
	glLoadIdentity();
	for (auto it = transforms->cbegin(); it != transforms->cend(); ++it)
	{
		WidgetTransform* t = static_cast<WidgetTransform*>(*it);
		Rect<float> r = t->getRect();

		float hue = float( (size_t(t)>>8&0xff) | ((size_t(t)>>0&0xff) << 8) ) / 0xffff;
		glColor4f(
			1-1.5f*std::min(abs(hue), abs(hue-1)),
			1-1.5f*abs(hue-0.33f),
			1-1.5f*abs(hue-0.66f),
			255
		);

		glLineWidth(3);
		glBegin(GL_LINE_STRIP);
		glVertex3f(r.topLeft      .x, r.topLeft      .y, 0);
		glVertex3f(r.topLeft      .x, r.bottomRight().y, 0);
		glVertex3f(r.bottomRight().x, r.bottomRight().y, 0);
		glVertex3f(r.bottomRight().x, r.topLeft      .y, 0);
		glVertex3f(r.topLeft      .x, r.topLeft      .y, 0);
		glEnd();
	}
	glColor4f(1, 1, 1, 1);
	// */

	glPopMatrix();
}

void HUD::raycast(Vector2f pos, const std::function<void(Widget*)>& visitor, bool exact) const
{
	std::vector<Widget*> hits;

	using _aliased_fp = bool (Widget::*)(Vector2f) const;
	const _aliased_fp fp_raycastExact = &Widget::raycastExact;
	widgets.staticCall(
		[&](Widget* w) //FIXME inefficient as heck, especially without cached transforms
		{
			bool didHit = exact ? (w->*fp_raycastExact)(pos) : w->transform->getRect().contains(pos);
			if (didHit) hits.push_back(w);
		}
	);
	
	std::sort(hits.begin(), hits.end(), [](Widget* a, Widget* b) { return a->transform->getRenderDepth() > b->transform->getRenderDepth(); });
	for (Widget* w : hits) if (!exact || w->raycastExact(pos)) visitor(w);
}

Widget* HUD::raycastClosest(Vector2f pos, bool exact) const
{
	Widget* out = nullptr;
	WidgetTransform::depth_t outDepth;
	
	using _aliased_fp = bool (Widget::*)(Vector2f) const;
	const _aliased_fp fp_raycastExact = &Widget::raycastExact;
	widgets.staticCall(
		[&](Widget* w) //FIXME inefficient as heck, especially without cached transforms
		{
			bool didHit = exact ? (w->*fp_raycastExact)(pos) : w->transform->getRect().contains(pos);
			if (didHit)
			{
				WidgetTransform::depth_t _depth = w->transform->getRenderDepth();
				if (!out) { out = w; outDepth = _depth; }
				else if (_depth > outDepth) { out = w; outDepth = _depth; }
			}
		}
	);
	
	return out;
}

WidgetTransform const* HUD::getRootTransform() const
{
	return root;
}

WidgetTransform* HUD::getRootTransform()
{
	return root;
}
