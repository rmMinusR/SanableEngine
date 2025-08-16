#include "game/GameWindowDispatcher.hpp"

#include <SDL_events.h>

#include "Window.hpp"
#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/CameraComponent.hpp"
#include "game/InputSystem.hpp"
#include "Renderer.hpp"
#include "Material.hpp"
#include "ShaderProgram.hpp"

GameWindowDispatcher::GameWindowDispatcher(Game* game) :
	game(game),
	hud(game)
{
}

GameWindowDispatcher::~GameWindowDispatcher()
{
}

void GameWindowDispatcher::handleEvent(const SDL_Event& ev)
{
	if (ev.type == SDL_WINDOWEVENT)
	{
		switch (ev.window.event)
		{
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			game->getInput()->onGainFocus();
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			game->getInput()->onLoseFocus();
			break;
		}
	}

	//game->getInput()->handleEvent(ev);
}
void GameWindowDispatcher::render(Rect<float> viewport)
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
	renderInterface->clear({ 0, 0, 0, 255 });

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

	ShaderUniform::GlobalData globalUniformData = renderInterface->getCurGlobalData();

	//Process buffer
	for (const auto& shaderGroup : renderables)
	{
		//Activate (or clear) shader and write global uniforms
		renderInterface->setActiveShader(shaderGroup.first);
		if (shaderGroup.first) shaderGroup.first->writeSharedUniforms(renderInterface, globalUniformData);

		for (const auto& materialGroup : shaderGroup.second)
		{
			//Activate material
			//if (materialGroup.first) materialGroup.first->writeUserSharedUniforms(renderInterface, userUniformLookup); // FIXME re-implement user uniforms
			assert(materialGroup.first == nullptr || materialGroup.first->getShader() == shaderGroup.first);

			for (const I3DRenderable* r : materialGroup.second)
			{
				r->loadModelTransform(renderInterface);

				if (materialGroup.first) materialGroup.first->writeInstanceUniforms(renderInterface, r->getRenderedInstanceUniforms());

				assert(r->getMaterial() == materialGroup.first);
				assert(r->getMaterial() == nullptr || r->getMaterial()->getShader() == shaderGroup.first);
				r->renderImmediate(renderInterface);
			}
		}
	}

	hud.refreshLayout(viewport);
	hud.tick(); //FIXME logic shouldn't be in render, move elsewhere
	hud.render(renderInterface);
}
