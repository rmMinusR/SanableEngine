#include "GLSettings.hpp"

#include <string.h>
#include <SDL_video.h>

bool GLSettings::operator==(const GLSettings& rhs) const
{
    return memcmp(this, &rhs, offsetof(GLSettings, doubleBuffer) + sizeof(doubleBuffer)) == 0;
}

void GLSettings::apply() const
{
    SDL_GL_ResetAttributes(); //Ensure clean state

	//Specify version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version.minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //Specify implicit framebuffer
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE  , colorBitsMin.r);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, colorBitsMin.g);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE , colorBitsMin.b);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, colorBitsMin.a);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthBitsMin);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doubleBuffer ? 1 : 0); //Instead of passing SDL_DOUBLEBUF to SDL_SetVideoMode
}
