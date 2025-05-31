cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_graphics_opengl_sources)
declare_dll("engine-graphics-opengl" "" engine_graphics_opengl_sources)
target_include_directories("engine-graphics-opengl" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

target_compile_definitions("engine-graphics-opengl" PRIVATE engine_graphics_IMPLEMENTATION) # Render interface symbols get bundled in with implementation

# Declare imports
target_include_directories("engine-graphics-opengl" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-graphics-opengl" PUBLIC engine-graphics-abstract engine-platform-abstract ${OPENGL_gl_LIBRARY} glew)

# Declare exports
target_include_directories("engine-graphics-opengl" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
stix_extract_ast(engine-graphics-opengl ${CMAKE_CURRENT_LIST_DIR})

export_resource(engine-graphics-opengl "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/shaders/font/frag.glsl)
export_resource(engine-graphics-opengl "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/shaders/font/vert.glsl)
export_resource(engine-graphics-opengl "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/shaders/image/frag.glsl)
export_resource(engine-graphics-opengl "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/shaders/image/vert.glsl)
