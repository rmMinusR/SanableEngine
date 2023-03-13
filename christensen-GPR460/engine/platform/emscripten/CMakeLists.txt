cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-vendor")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_vendor_sources)
add_executable("engine-vendor" ${engine_vendor_sources})
target_include_directories("engine-vendor" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

if (EMSCRIPTEN)
	set_target_properties(engine-vendor PROPERTIES
		OUTPUT_NAME "index"
		SUFFIX ".html"
	)
	target_link_options(engine-vendor PRIVATE "--shell-file ${CMAKE_CURRENT_LIST_DIR}/frontend.html")
	#target_link_options(engine-vendor PRIVATE "--embed-file ${RUNTIME_OUTPUT_DIRECTORY}/plugins@plugins")
endif()

# Declare post-build action: Copy SDL2.dll to output dir
export_dll(SDL2::SDL2 engine-vendor)

# Declare imports
target_link_libraries("engine-vendor" "engine-core")
