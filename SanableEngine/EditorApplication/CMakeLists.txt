cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" EditorApplication_sources)
add_sanable_executable(EditorApplication "${EditorApplication_sources}")

target_include_directories(EditorApplication PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

export_resource(EditorApplication "${CMAKE_CURRENT_LIST_DIR}" "" resources/ui/fonts/arial.ttf)
