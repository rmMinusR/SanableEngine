cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("engine-emscripten")

# Declare target
aux_source_directory("${CMAKE_CURRENT_LIST_DIR}/src" engine_emscripten_sources)
add_library("engine-emscripten" STATIC ${engine_emscripten_sources})
target_link_options("engine-emscripten" PRIVATE -sMAIN_MODULE)
target_include_directories("engine-emscripten" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/private")
target_include_directories("engine-emscripten" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/public")

# Debug options
#target_link_options(engine-emscripten PUBLIC --cpuprofiler)

# Declare imports
target_link_options(engine-emscripten PUBLIC -sUSE_SDL=2)
target_link_libraries("engine-emscripten" "engine-core")

function(package_assets)
	message("Detected plugins ${sanableAllPlugins}")
	add_dependencies("engine-emscripten" ${sanableAllPlugins}) # Ensure plugins make it into index.data first time around
	#set(file_packager "${CMAKE_SOURCE_DIR}/../emsdk/upstream/emscripten/tools/file_packager.py")
	#add_custom_target("index.data" python ${file_packager} ${CMAKE_CURRENT_BINARY_DIR}/index.data --preload ${CMAKE_CURRENT_BINARY_DIR}/plugins@/plugins --wasm64 --use-preload-plugins
	#				DEPENDS ${sanableAllPlugins})
	#add_dependencies("engine-emscripten" "index.data")
endfunction()

function(add_sanable_executable targetName sources)
	add_executable(${targetName} ${sources})
	target_link_libraries(${targetName} PRIVATE engine-emscripten)
	
	# Declare install targets
	install(FILES "$<TARGET_FILE_DIR:${targetName}>/index.html" "$<TARGET_FILE_DIR:${targetName}>/index.js" "$<TARGET_FILE_DIR:${targetName}>/index.wasm" "$<TARGET_FILE_DIR:${targetName}>/index.data" DESTINATION "./")

	# Fix outputs
	set_target_properties(${targetName} PROPERTIES
		OUTPUT_NAME "index"
		SUFFIX ".html"
	)
	set_property(TARGET "${targetName}" APPEND PROPERTY ADDITIONAL_CLEAN_FILES "index.data" "index.wasm" "index.js")

	# Additional assets
	#target_link_options(${targetName} PRIVATE --shell-file ${CMAKE_CURRENT_LIST_DIR}/frontend.html)
	#target_link_options(${targetName} PRIVATE --embed-file ${CMAKE_CURRENT_BINARY_DIR}/plugins@/plugins)
	target_link_options(${targetName} PRIVATE --preload-file ${CMAKE_CURRENT_BINARY_DIR}/plugins@/plugins)
	target_link_options(${targetName} PRIVATE -sFORCE_FILESYSTEM)

endfunction()
