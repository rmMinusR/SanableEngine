#include "game/GameWindowRenderPipeline.hpp"

#include <unordered_map>
#include <GL/glew.h>
#include "Window.hpp"
#include "game/Game.hpp"
#include "game/Level.hpp"
#include "game/CameraComponent.hpp"
#include "Renderer.hpp"
#include "Material.hpp"
#include "ShaderProgram.hpp"
#include "MemoryRoot.hpp"

GameWindowRenderPipeline::GameWindowRenderPipeline(Game* game) :
	game(game),
	hud(game->getApplication())
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
	Renderer* renderInterface = window->getRenderer();

	//Set projection matrix
	CameraComponent* cam = CameraComponent::getMain();
	if (cam)
	{
		const Transform* camTransform = cam->getGameObject()->getTransform();
		renderInterface->beginFrame(*cam->getConfig(), viewport, camTransform->getPosition(), camTransform->getRotation());
	}
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
	> renderables; //Note: No need for a CallBatcher here, we're guaranteed renderables will be grouped by level, then type since our data source is a CallBatcher
	auto registerRenderable = [&](const I3DRenderable* r) { renderables[r->getShader()][r->getMaterial()].push_back(r); };
	game->visitLevels([&](Level* level) { level->get3DRenderables()->staticCall(registerRenderable); });

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	ShaderUniform::GlobalData globalUniformData = renderInterface->getCurGlobalData();

	//Process buffer
	for (const auto& shaderGroup : renderables)
	{
		//Activate (or clear) shader and write global uniforms
		renderInterface->setActiveShader(shaderGroup.first);
		if(shaderGroup.first) shaderGroup.first->writeSharedUniforms(renderInterface, globalUniformData);

		for (const auto& materialGroup : shaderGroup.second)
		{
			//Activate material
			//if (materialGroup.first) materialGroup.first->writeUserSharedUniforms(renderInterface, userUniformLookup); // FIXME re-implement user uniforms
			assert(materialGroup.first == nullptr || materialGroup.first->getShader() == shaderGroup.first);

			for (const I3DRenderable* r : materialGroup.second)
			{
				r->loadModelTransform(renderInterface);

				if (materialGroup.first) materialGroup.first->writeInstanceUniforms(renderInterface, r);

				assert(r->getMaterial() == materialGroup.first);
				assert(r->getMaterial() == nullptr || r->getMaterial()->getShader() == shaderGroup.first);
				r->renderImmediate(renderInterface);
			}
		}
	}

	glPopMatrix();

	hud.refreshLayout(viewport);
	hud.tick(); //FIXME logic shouldn't be in render, move elsewhere
	hud.render(renderInterface);
}
