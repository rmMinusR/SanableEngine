cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" editor_shared_sources)
declare_dll("editor-shared" "" editor_shared_sources)
target_include_directories(editor-shared PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare imports
target_link_libraries(editor-shared PUBLIC engine-core)

# Declare exports
target_include_directories(editor-shared PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare install targets
install_dll("editor-shared" ".")

#stix_use_template(engine-shared "${CMAKE_CURRENT_LIST_DIR}/rtti.template.cpp")
#stix_default_image_capture_status(engine-shared disabled)
#stix_extract_ast(engine-shared ${CMAKE_CURRENT_LIST_DIR})
#stix_generate_reflection(engine-shared)

# Declare resources
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/fonts/arial.ttf)
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_v/normal.png)
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_v/dragged.png)
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_h/normal.png)
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_h/dragged.png)
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/placeholder_1.png)
export_resource(editor-shared "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/placeholder_2.png)
