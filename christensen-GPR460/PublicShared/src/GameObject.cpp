#include "GameObject.hpp"

#include <utility>
#include <cassert>

#include "EngineCore.hpp"
#include "Component.hpp"
#include "Hotswap.hpp"

void GameObject::BindComponent(Component* c)
{
	assert(c->gameObject == nullptr || c->gameObject == this);
	assert(std::find_if(components.cbegin(), components.cend(), [=](GameObject::ComponentRecord i) { return i.ptr == c; }) == components.cend());

	ComponentRecord record;
	record.ptr = c;
	//r.name = ???
	components.push_back(record);
	c->BindToGameObject(this);

	IUpdatable* u = dynamic_cast<IUpdatable*>(c);
	if (u) engine->updateList.add(u);

	IRenderable* r = dynamic_cast<IRenderable*>(c);
	if (r) engine->renderList.add(r);
}

void GameObject::InvokeStart()
{
	for (ComponentRecord& c : components) c.ptr->onStart();
}

GameObject::GameObject(EngineCore* engine) :
	transform(0, 0, 0),
	engine(engine)
{
}

GameObject::~GameObject()
{
	if (components.size() != 0)
	{
		for (ComponentRecord& c : components) engine->getMemoryManager()->destroy(c.ptr);
		components.clear();
	}
}

void GameObject::hotswapRefresh(std::vector<HotswapTypeData*>& refreshers)
{
	for (ComponentRecord& c : components)
	{
		auto it = std::find_if(refreshers.cbegin(), refreshers.cend(), [=](HotswapTypeData* d) { return d->name == c.name; });
		set_vtable_ptr(c.ptr, (*it)->vtable);
	}
}
