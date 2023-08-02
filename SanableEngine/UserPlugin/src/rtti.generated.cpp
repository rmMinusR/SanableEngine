//Common includes
#include "PluginCore.hpp"
#include "TypeBuilder.hpp"
#include "TypedMemoryPool"

//Dependency includes
#include "ColliderColorChanger.hpp"
#include "PlayerController.hpp"

PLUGIN_C_API(void) plugin_reportTypes(ModuleTypeRegistry* registry)
{
    //ColliderColorChanger
    {
        TypeBuilder builder = TypeBuilder::fromCDO<ColliderColorChanger>(false, SDL_Color(), SDL_Color());
        builder.addParent(TypeName("Component"));
        builder.addParent(TypeName("IUpdatable"));
        builder.addField(TypeName("SDL_Color", 0), "normalColor");
        builder.addField(TypeName("SDL_Color", 0), "overlapColor");
        builder.addField(TypeName("RectangleCollider", 1), "collider");
        builder.addField(TypeName("RectangleRenderer", 1), "renderer");
        builder.registerType(registry);
    }

    //types.push_back(TypeInfo::build<TypedMemoryPool<TObj>>(1));

    types->registerType<PlayerController>();
}
