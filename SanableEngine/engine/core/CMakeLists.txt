cmake_minimum_required (VERSION 3.11)
set (CMAKE_CXX_STANDARD 17)

project("engine-core")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_core_sources)
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src/application" engine_core_application_sources)
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src/game" engine_core_game_sources)
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src/gui" engine_core_gui_sources)
list(APPEND engine_core_sources ${engine_core_application_sources} ${engine_core_game_sources} ${engine_core_gui_sources})
declare_dll("engine-core" "" engine_core_sources)
target_include_directories("engine-core" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_include_directories("engine-core" PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries("engine-core" PUBLIC ${SDL2_LIBRARIES} engine-memory engine-platform-abstract engine-assets glm::glm)

# Declare exports
target_include_directories("engine-core" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")
target_include_directories("engine-core" INTERFACE "${CMAKE_CURRENT_LIST_DIR}/interface")

# Declare install targets
install_dll("engine-core" ".")

stix_use_template(engine-core "${CMAKE_CURRENT_LIST_DIR}/template/rtti.template.cpp")
stix_default_image_capture_status(engine-core disabled)
stix_generate_reflection(engine-core ${CMAKE_CURRENT_LIST_DIR})
