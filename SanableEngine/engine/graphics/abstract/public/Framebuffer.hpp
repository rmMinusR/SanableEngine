#pragma once

#include "math/Vector2.inl"

class GTexture;

class Framebuffer
{
public:
	struct Settings
	{
		size_t numChannels = 4;
		bool depthBuffer = true;
	};

	virtual ~Framebuffer() = default;

	virtual void resize(Vector2<int> size) = 0;
	virtual operator bool() const = 0;

	virtual const GTexture* getTexture() const = 0;
	virtual GTexture* getTexture() = 0;
};
