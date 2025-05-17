#pragma once

#include <vector>
#include <string>

class MenuButton;
class MenuDivider;
class MenuItem;


class MenuItemFactory
{
public:
	virtual ~MenuItemFactory() = default;
	virtual MenuButton* addButton(MenuItem* parent, std::wstring text, size_t index = 0) = 0; // Does NOT add to parent's child list
	virtual MenuDivider* addDivider(MenuItem* parent, size_t index = 0) = 0; // Does NOT add to parent's child list
};


class MenuItem
{
protected:
	MenuItemFactory* itemFactory;
	MenuItem* parent = nullptr;
	std::vector<MenuItem*> children;

	virtual void addItem(MenuItem* item, size_t index);

	MenuItem(MenuItemFactory* itemFactory);
public:
	virtual ~MenuItem();

	size_t getIndexInParent() const;
	MenuItem* getParent();
	const MenuItem* getParent() const;

	virtual size_t getNumChildren() const;
	virtual MenuItem* getChild(size_t which);
	virtual const MenuItem* getChild(size_t which) const;

	virtual MenuButton* addButton(std::wstring text, size_t index = 0);
	virtual MenuDivider* addDivider(size_t index = 0);
};
