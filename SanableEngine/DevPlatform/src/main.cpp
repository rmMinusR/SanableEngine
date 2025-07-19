#include <iostream>
#include <string>

#include "MemoryRoot.hpp"
#include "System.hpp"
#include "game/Game.hpp"
#include "game/GameWindowDispatcher.hpp"

void SanableMain(gpr460::System* system)
{
    // Init
    Game game(*system);
    MemoryRoot::get()->registerExternal(&game, ExternalObjectOptions::DefaultExternal);
    game.init();

    // Setup game window
    {
        constexpr int WIDTH = 640;
        constexpr int HEIGHT = 480;
        WindowSettings mainWindowSettings("Sanable Engine", WIDTH, HEIGHT);
        mainWindowSettings.userLogic = new GameWindowDispatcher(&game);

        Window* gameWindow = game.buildWindow(mainWindowSettings);
        game.setMainWindow(gameWindow);
    }

    // Setup plugins
    {
        PluginManager& pluginManager = *game.getPluginManager();

        // Discover all plugins
        for (const std::filesystem::path& dllPath : system->ListPlugins(system->GetBaseDir() / "plugins"))
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
    game.getHeap()->ensureFresh();
    game.doMainLoop();

    // Shutdown
    game.cleanup();
}
