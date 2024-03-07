#include "game/gui/HUD.hpp"

#include <unordered_map>

#include "ShaderProgram.hpp"
#include "Material.hpp"

HUD::HUD()
{
}

HUD::~HUD()
{
}

void HUD::render(Renderer* renderer)
{
	//Collect objects to buffer
	std::unordered_map<
		const ShaderProgram*, //Group by shader
		std::unordered_map<
			const Material*, //Then by material
			std::vector<Widget*>
		>
	> renderables;
	widgets.staticCall([&](Widget* w)
	{
		renderables[w->getShader()][w->getMaterial()].push_back(w);
	});

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
				if (materialGroup.first) materialGroup.first->writeInstanceUniforms(renderer, w);

				assert(w->getMaterial() == materialGroup.first);
				assert(w->getMaterial() == nullptr || w->getMaterial()->getShader() == shaderGroup.first);
				w->renderImmediate(renderer);
			}
		}
	}
}
