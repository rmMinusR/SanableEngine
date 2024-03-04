#pragma once

#include "dllapi.h"

struct GLSettings
{
	//Default: v3.1 (latest with fixed-function pipeline)
	struct
	{
		int major = 3;
		int minor = 1;
	} version;

	//Default: RGBA8 color
	struct
	{
		int r = 8;
		int g = 8;
		int b = 8;
		int a = 8;
	} colorBitsMin;

	//Default: 16-bit depth
	int depthBitsMin = 16;

	bool doubleBuffer = true;

	ENGINEGRAPHICS_API bool operator==(const GLSettings& rhs) const;
	ENGINEGRAPHICS_API void apply() const;
};
