#pragma once

#include "MenuContainer.hpp"

class MenuItem : public MenuContainer
{
protected:
	MenuItem* parent = nullptr;

	MenuItem();
public:
	virtual ~MenuItem();

	size_t getIndexInParent() const;
	MenuItem* getParent();
	const MenuItem* getParent() const;
};
