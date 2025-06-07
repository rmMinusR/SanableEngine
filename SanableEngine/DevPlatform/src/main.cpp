#include <iostream>
#include <string>

#include "System.hpp"
#include "application/Application.hpp"
#include "game/Game.hpp"
#include "game/GameWindowDispatcher.hpp"

void SanableMain(Application* application)
{
    // Setup game window
    {
        constexpr int WIDTH = 640;
        constexpr int HEIGHT = 480;
        WindowSettings mainWindowSettings("Sanable Engine", WIDTH, HEIGHT);
        mainWindowSettings.userLogic = new GameWindowDispatcher(application->getGame());

        Window* gameWindow = application->buildWindow(mainWindowSettings);
        application->setMainWindow(gameWindow);
    }

    // Setup plugins
    {
        gpr460::System* system = application->getSystem();
        PluginManager& pluginManager = *application->getPluginManager();

        // Discover all plugins
        for (const std::filesystem::path& dllPath : application->getSystem()->ListPlugins(system->GetBaseDir() / "plugins"))
        {
            pluginManager.discover(dllPath);
        }
        std::cout << "Discovered " << pluginManager.getNumPlugins() << " plugins" << std::endl;
        pluginManager.enumeratePlugins(
            [system](Plugin* p) {
                std::cout << " - " << std::filesystem::relative(p->getPluginDir(), system->GetBaseDir()).string() << std::endl;
            }
        );

        // Load
        // Note this submits to a buffer, does not load immediately
        pluginManager.enumeratePlugins([&pluginManager](Plugin* p) { pluginManager.load(p); }); // TODO use dependency graph instead
        pluginManager.enumeratePlugins([&pluginManager](Plugin* p) { pluginManager.hook(p); });
    }

    // Run
    application->getHeap()->ensureFresh();
	application->doMainLoop();
}
