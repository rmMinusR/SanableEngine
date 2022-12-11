#include "PluginManager.hpp"

#include <sstream>
#include <cassert>

PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
	assert(plugins.size() == 0);
}

void PluginManager::discoverAll(const std::wstring& pluginsFolderPath)
{
	std::wostringstream joiner;

	//Discover contents
	std::vector<std::wstring> contents;
	WIN32_FIND_DATA it;
	HANDLE hFind = FindFirstFile(pluginsFolderPath.c_str(), &it);
	do
	{
		if (it.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//joiner << pluginsFolderPath << L'\\';
			joiner << it.cFileName << L'\\';
			joiner << it.cFileName << L".dll";

			contents.push_back(joiner.str());
			joiner.clear();
		}
	}
	while (FindNextFile(hFind, &it));
	bool cleanupSuccess = FindClose(hFind);
	assert(cleanupSuccess);

	//Create a wrapper and load the DLL of each
	std::vector<Plugin*> batch;
	for (const std::wstring& dllPath : contents)
	{
		Plugin* p = &plugins.emplace_back(dllPath);
		p->loadDLL();
		batch.push_back(p); //Defer registerContents call
	}

	//Load contents of each
	for (Plugin* p : batch) p->registerContents();
}

void PluginManager::load(const std::wstring& dllPath)
{
	Plugin* p = &plugins.emplace_back(dllPath);
	p->loadDLL();
	p->registerContents();
}
