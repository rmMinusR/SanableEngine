cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-win32")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_win32_sources)
add_executable("engine-win32" ${engine_win32_sources})
target_include_directories("engine-win32" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")

# Declare post-build action: Copy SDL2.dll to output dir
export_dll(SDL2::SDL2 engine-win32)

# Declare imports
target_link_libraries("engine-win32" "engine-core")

function(package_assets)
	#Nothing to do, plugins are responsible for copying their assets to their output directory
endfunction()

# Declare install targets
install(TARGETS "engine-win32"
	RUNTIME DESTINATION "." COMPONENT Runtime
	LIBRARY DESTINATION "." COMPONENT Runtime
)
install(FILES "$<TARGET_FILE:SDL2::SDL2>" DESTINATION ".")
