#pragma once

#include <functional>

#include "MenuItem.hpp"

class MenuDivider : public MenuItem
{
protected:
	MenuDivider();

public:
	virtual ~MenuDivider() = default;

	virtual size_t getNumChildren() const override;
	virtual MenuItem* getChild(size_t which) override;
	virtual const MenuItem* getChild(size_t which) const override;

	virtual MenuButton* addButton(std::wstring text, size_t index = 0) override;
	virtual MenuDivider* addDivider(size_t index = 0) override;
};
