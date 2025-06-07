cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-win32")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_win32_sources)
add_library("engine-win32" STATIC ${engine_win32_sources})
target_include_directories("engine-win32" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")
target_include_directories("engine-win32" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Declare imports
target_link_libraries("engine-win32" engine-core engine-graphics-opengl)

function(package_assets)
	#Nothing to do, plugins are responsible for copying their assets to their output directory
	message("Detected plugins ${sanableAllPlugins}")
endfunction()

function(add_sanable_executable targetName sources)
	add_executable(${targetName} WIN32 ${sources})
	target_link_libraries(${targetName} PRIVATE engine-win32)

	# Declare post-build action: Copy SDL2.dll to output dir
	export_dll(SDL2::SDL2 ${targetName})
	export_dll(SDL2_image::SDL2_image ${targetName})
	#export_dll(SDL2_ttf::SDL2_ttf ${targetName})

	# Declare install targets
	install(TARGETS "${targetName}"
		RUNTIME DESTINATION "." COMPONENT Runtime
		LIBRARY DESTINATION "." COMPONENT Runtime
	)
	install(FILES "$<TARGET_FILE:SDL2::SDL2>" DESTINATION ".")
endfunction()
