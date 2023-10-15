#include "GLContext.hpp"

#include <cassert>
#include <GL/glew.h>

std::unordered_map<SDL_GLContext, int> GLContext::handles; //ptr -> refcount
bool GLContext::glewGood;

SDL_GLContext GLContext::create(SDL_Window* window, void* who)
{
	SDL_GLContext ctx = SDL_GL_CreateContext(window);
	handles[ctx]++; //Update refcount. No need to do special handling.

	//Setup GLEW
	if (!glewGood)
	{
		GLenum status = glewInit(); //TODO would this need to be called again for higher GL versions?
		if (status == GLEW_OK) glewGood = true;
		else
		{
			printf("Error initializing GLEW: Code %u", status);
			assert(false);
		}
	}

	return ctx;
}

void GLContext::release(SDL_GLContext ctx, void* who)
{
	auto it = handles.find(ctx);
	assert(it != handles.end());
	it->second--; //Update refcount

	if (it->second <= 0)
	{
		SDL_GL_DeleteContext(ctx); //Destroy if all references gone
		handles.erase(it);
	}

	if (handles.size() == 0)
	{
		
	}
}
