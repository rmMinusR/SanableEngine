cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" EditorApplication_sources)
add_sanable_executable(EditorApplication "${EditorApplication_sources}")

target_include_directories(EditorApplication PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/fonts/arial.ttf)
export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_v/normal.png)
export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_v/dragged.png)
export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_h/normal.png)
export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/divider_h/dragged.png)
export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/placeholder_1.png)
export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/textures/placeholder_2.png)
