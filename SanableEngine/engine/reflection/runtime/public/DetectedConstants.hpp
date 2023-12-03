#pragma once

#include <vector>

#include "dllapi.h"


struct DetectedConstants
{
	std::vector<uint8_t> bytes;
	std::vector<bool> usage;

	ENGINE_RTTI_API DetectedConstants(size_t sz);
	ENGINE_RTTI_API void resize(size_t sz);

	//Keeps our detected constants only if our counterpart has also
	//detected the same constant. Used for vtable detection.
	ENGINE_RTTI_API void merge(const DetectedConstants& other);

	ENGINE_RTTI_API DetectedConstants();
	ENGINE_RTTI_API ~DetectedConstants();
	ENGINE_RTTI_API DetectedConstants(const DetectedConstants& cpy);
	ENGINE_RTTI_API DetectedConstants(DetectedConstants&& mov);
	ENGINE_RTTI_API DetectedConstants& operator=(const DetectedConstants& cpy);
	ENGINE_RTTI_API DetectedConstants& operator=(DetectedConstants&& mov);
};

DetectedConstants platform_captureConstants(size_t objSize, void(*ctor)());
