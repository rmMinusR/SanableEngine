cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

# Allow pseudo-blocking operations
target_link_options("engine-core" PUBLIC -sASYNCIFY)

project("engine-emscripten")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_emscripten_sources)
add_executable("engine-emscripten" ${engine_emscripten_sources})
target_link_options("engine-emscripten" PRIVATE -sMAIN_MODULE)
target_include_directories("engine-emscripten" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Fix outputs
set_target_properties(engine-emscripten PROPERTIES
	OUTPUT_NAME "index"
	SUFFIX ".html"
)

# Additional assets
#target_link_options(engine-emscripten PRIVATE --shell-file ${CMAKE_CURRENT_LIST_DIR}/frontend.html)
#target_link_options(engine-emscripten PRIVATE --embed-file ${CMAKE_CURRENT_BINARY_DIR}/plugins@/plugins)
target_link_options(engine-emscripten PRIVATE --preload-file ${CMAKE_CURRENT_BINARY_DIR}/plugins@/plugins)

# Declare imports
target_link_libraries("engine-emscripten" "engine-core")
