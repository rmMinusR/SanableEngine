#pragma once

#include "MenuContainer.hpp"

class MenuItem
{
protected:
	friend class MenuContainer;
	MenuContainer* parent = nullptr;

	MenuItem();
public:
	virtual ~MenuItem();

	size_t getIndexInParent() const;
	MenuContainer* getParent();
	const MenuContainer* getParent() const;
};
