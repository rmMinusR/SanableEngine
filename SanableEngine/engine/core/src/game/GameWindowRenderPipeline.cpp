#include "game/GameWindowRenderPipeline.hpp"

#include <unordered_map>
#include <GL/glew.h>
#include "game/Game.hpp"
#include "application/Window.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "ShaderProgram.hpp"

GameWindowRenderPipeline::GameWindowRenderPipeline(Game* game) :
	game(game)
{
}

GameWindowRenderPipeline::~GameWindowRenderPipeline()
{
}

void GameWindowRenderPipeline::setup(Window* window)
{
	WindowRenderPipeline::setup(window);
}

void GameWindowRenderPipeline::render(Rect<float> viewport)
{
	//Set projection matrix
	if (Camera::getMain()) Camera::getMain()->beginFrame({ viewport.width, viewport.height, 0 });
	else printf("WARNING: No main camera!");

	//Reset screen
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Collect objects to buffer
	std::unordered_map<
		const ShaderProgram*, //Group by shader
		std::unordered_map<
			const Material*, //Then by material
			std::vector<const I3DRenderable*>
		>
	> renderables;
	game->get3DRenderables()->staticCall([&](I3DRenderable* r)
	{
		Material* material = r->getMaterial();
		const ShaderProgram* shader = material ? material->getShader() : nullptr;
		renderables[shader][material].push_back(r);
	});

	//Process buffer
	Renderer* renderInterface = window->getRenderer();
	for (const auto& shaderGroup : renderables)
	{
		//Activate shader
		if (shaderGroup.first) shaderGroup.first->activate();
		else ShaderProgram::clear();

		for (const auto& materialGroup : shaderGroup.second)
		{
			//Activate material
			if (materialGroup.first) materialGroup.first->writeSharedUniforms(renderInterface);
			assert(materialGroup.first == nullptr || materialGroup.first->getShader() == shaderGroup.first);

			for (const I3DRenderable* r : materialGroup.second)
			{
				if (materialGroup.first) materialGroup.first->writeInstanceUniforms(renderInterface, r);

				assert(r->getMaterial() == materialGroup.first);
				assert(r->getMaterial() == nullptr || r->getMaterial()->getShader() == shaderGroup.first);
				r->renderImmediate(renderInterface);
			}
		}
	}
}