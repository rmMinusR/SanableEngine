#pragma once

#include <vector>
#include <string>

class MenuButton;
class MenuDivider;
class MenuItem;

class MenuItem
{
protected:
	MenuItem* parent = nullptr;
	std::vector<MenuItem*> children;

	virtual void addItem(MenuItem* item, size_t index); // Internal helper

	MenuItem();
public:
	virtual ~MenuItem();

	size_t getIndexInParent() const;
	MenuItem* getParent();
	const MenuItem* getParent() const;

	virtual size_t getNumChildren() const;
	virtual MenuItem* getChild(size_t which);
	virtual const MenuItem* getChild(size_t which) const;

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) = 0;
	virtual MenuDivider* addDivider(size_t index = 0) = 0;

	virtual void* getNativeHandle() = 0;
	virtual const void* getNativeHandle() const = 0;
};
