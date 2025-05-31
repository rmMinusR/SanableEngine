cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_graphics_abstract_sources)
#add_library("engine-graphics-abstract" STATIC ${engine_graphics_abstract_sources})
declare_dll("engine-graphics-abstract" "" engine_graphics_abstract_sources)
target_include_directories("engine-graphics-abstract" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

target_compile_definitions("engine-graphics-abstract" PRIVATE engine_graphics_abstract_EXPORTS) # Why isn't this already defined??

# Declare imports
target_include_directories("engine-graphics-abstract" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-graphics-abstract" PUBLIC engine-memory engine-common ${SDL2_LIBRARIES} OpenFBX stb_image freetype SDL2_image::SDL2_image)

# Declare exports
target_include_directories("engine-graphics-abstract" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

stix_use_template(engine-graphics-abstract "${CMAKE_CURRENT_LIST_DIR}/rtti.template.cpp")
stix_default_image_capture_status(engine-graphics-abstract disabled)
stix_extract_ast(engine-graphics-abstract ${CMAKE_CURRENT_LIST_DIR})
stix_generate_reflection(engine-graphics-abstract)
