cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_gui_sources)
target_sources("engine-core" PRIVATE ${engine_gui_sources}) # Bundle in with engine-core
#declare_dll("engine-gui" "" engine_gui_sources)
target_include_directories("engine-core" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-core" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-core" PUBLIC ${SDL2_LIBRARIES} ${OPENGL_gl_LIBRARY} glew stb_image engine-memory SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

# Declare exports
target_include_directories("engine-core" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
generate_reflection(engine-core ${CMAKE_CURRENT_LIST_DIR})
