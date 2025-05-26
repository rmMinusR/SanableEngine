#pragma once

#include <vector>
#include <string>


class MenuItem;
class MenuButton;
class MenuDivider;
class Submenu;


class MenuContainer
{
protected:
	std::vector<MenuItem*> children;
	MenuContainer();

	virtual void registerItem(MenuItem* item, size_t index); // Internal helper

public:
	virtual ~MenuContainer();

	virtual size_t getNumChildren() const;
	virtual MenuItem* getChild(size_t which);
	virtual const MenuItem* getChild(size_t which) const;

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) = 0;
	virtual Submenu* addSubmenu(std::wstring text, size_t index = 0) = 0;
	virtual MenuDivider* addDivider(size_t index = 0) = 0;

	virtual void* getNativeHandle() = 0;
	virtual const void* getNativeHandle() const = 0;
};
