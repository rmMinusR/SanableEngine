cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-core")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_core_sources)
declare_dll("engine-core" "" engine_core_sources)
target_include_directories("engine-core" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-core" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-core" PUBLIC ${SDL2_LIBRARIES} engine-memory engine-platform-abstract engine-assets)

# Declare exports
target_include_directories("engine-core" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare install targets
install_dll("engine-core" ".")

set_target_properties(engine-core PROPERTIES RTTI_TEMPLATE_FILE "${CMAKE_CURRENT_LIST_DIR}/template/rtti.template.cpp")
generate_reflection(engine-core ${CMAKE_CURRENT_LIST_DIR})
