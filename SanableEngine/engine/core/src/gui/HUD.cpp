#include "gui/HUD.hpp"

#include <unordered_map>
#include <algorithm>

#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Renderer.hpp"

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

void HUD::destroyWidget_internal(Widget* widget)
{
	removeQueue.push_back(widget);
}

HUD::HUD(Application* application) :
	application(application)
{
	root = memory.create<WidgetTransform>(nullptr, this);
	root->setPositioningStrategy_internal(nullptr);
	root->setRelativeRenderDepth(-500); //This should give us a depth range of 500 in either direction on most setups. FIXME magic number

	transforms = memory.getSpecificPool<WidgetTransform>(true);
}

HUD::~HUD()
{
	memory.destroy(root);
}

MemoryHeap* HUD::getMemory()
{
	return &memory;
}

void HUD::refreshLayout(Rect<float> viewport)
{
	if (viewport != root->rect)
	{
		root->rect = root->localRect = viewport;
		root->markDirty();
	}

	applyConcurrencyBuffers();
	for (auto it = transforms->cbegin(); it != transforms->cend(); ++it) it->refresh();
	applyConcurrencyBuffers();
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
		Material::Group, //Group by material class
		std::vector<Widget*> //Then order by depth
	> renderables; //Note: no point in grouping by material
	widgets.staticCall([&](Widget* w)
	{
		const Material* m = w->getMaterial();
		Material::Group group = m ? m->getGroup() : Material::Group::Opaque;
		renderables[group].push_back(w);
	});
	{
		//Opaque objects should draw front-to-back so we can discard occluded fragments faster
		auto it = renderables.find(Material::Group::Opaque);
		if (it != renderables.end())
		{
			std::sort(it->second.begin(), it->second.end(),
				[](const Widget* a, const Widget* b)
				{
					return std::greater<WidgetTransform::depth_t>{}(a->getTransform()->getRenderDepth(), b->getTransform()->getRenderDepth());
				}
			);
		}

		//Transparent/blended objects should draw back-to-front so we have correct color accuracy
		it = renderables.find(Material::Group::Transparent);
		if (it != renderables.end())
		{
			std::sort(it->second.begin(), it->second.end(),
				[](const Widget* a, const Widget* b)
				{
					return std::less<WidgetTransform::depth_t>{}(a->getTransform()->getRenderDepth(), b->getTransform()->getRenderDepth());
				}
			);
		}
	}

	//Process buffer
	auto processMaterialClass = [&](Material::Group _class)
	{
		renderer->errorCheck();
		for (Widget* w : renderables[_class])
		{
			if ((w->getTransform()->getVisibility() & WidgetVisibility::FLAGS_Renders) == WidgetVisibility::FLAGS_Renders)
			{
				//Activate shader
				renderer->setActiveShader(w->getShader());
				renderer->errorCheck();

				//Activate material
				const Material* mat = w->getMaterial();

				if (mat) mat->getShader()->writeSharedUniforms(renderer, renderer->getCurGlobalData());
				renderer->errorCheck();

				// FIXME user uniforms

				w->loadModelTransform(renderer);
				renderer->errorCheck();
				if (mat) mat->writeInstanceUniforms(renderer, w->getRenderedInstanceUniforms());
				renderer->errorCheck();

				w->renderImmediate(renderer);
				renderer->errorCheck();
			}
		}
	};
	processMaterialClass(Material::Group::Opaque);
	processMaterialClass(Material::Group::Transparent);

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
}

size_t HUD::raycast(Vector2f pos, WidgetTransform** hitsOut, size_t hitsOutMaxSz, bool exact) const
{
	size_t idx = 0;

	for (auto it = transforms->cbegin(); it != transforms->cend(); ++it)
	{
		if ((it->getVisibility() & WidgetVisibility::FLAGS_Raycastable) != WidgetVisibility::FLAGS_Raycastable)
			continue;

		Widget* w = it->getWidget();
		if ((w && exact) ? w->raycastExact(pos) : it->getRect().contains(pos))
		{
			if(hitsOut && idx < hitsOutMaxSz) hitsOut[idx] = &*it;
			idx++;
		}
	}

	if (hitsOut)
	{
		std::sort(
			hitsOut,
			hitsOut + std::min(idx, hitsOutMaxSz),
			[](WidgetTransform* a, WidgetTransform* b) { return a->getRenderDepth() > b->getRenderDepth(); }
		);
	}

	return idx;
}

void HUD::raycast(Vector2f pos, const std::function<void(WidgetTransform*)>& visitor, bool exact) const
{
	std::vector<WidgetTransform*> hits;

	for (auto it = transforms->cbegin(); it != transforms->cend(); ++it)
	{
		if ((it->getVisibility() & WidgetVisibility::FLAGS_Raycastable) != WidgetVisibility::FLAGS_Raycastable)
			continue;

		Widget* w = it->getWidget();
		if ((w && exact) ? w->raycastExact(pos) : it->getRect().contains(pos))
		{
			hits.push_back(&*it);
		}
	}
	
	std::sort(hits.begin(), hits.end(), [](WidgetTransform* a, WidgetTransform* b) { return a->getRenderDepth() > b->getRenderDepth(); });
	for (WidgetTransform* t : hits) visitor(t);
}

WidgetTransform* HUD::raycastClosest(Vector2f pos, bool exact) const
{
	WidgetTransform* out = nullptr;
	WidgetTransform::depth_t outDepth;
	
	for (auto it = transforms->cbegin(); it != transforms->cend(); ++it)
	{
		if ((it->getVisibility() & WidgetVisibility::FLAGS_Raycastable) != WidgetVisibility::FLAGS_Raycastable)
			continue;

		Widget* w = it->getWidget();
		if ((w && exact) ? w->raycastExact(pos) : it->getRect().contains(pos))
		{
			WidgetTransform::depth_t _depth = it->getRenderDepth();
			if (!out || _depth > outDepth) { out = &*it; outDepth = _depth; }
		}
	}

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

Application* HUD::getApplication() const
{
	return application;
}
