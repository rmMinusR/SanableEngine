cmake_minimum_required (VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

project("engine-reflection-tools")

option(STIX_RTTIGEN_PROFILING "Enable profiling during reflection generation time" FALSE)

# Optional property setters. Must be set before calling stix_generate_reflection.

function(stix_use_template targetName template)
	set_target_properties(${targetName} PROPERTIES STIX_TEMPLATE_FILE ${template})
endfunction()

function(stix_default_image_capture_backend targetName backend)
	set_target_properties(${targetName} PROPERTIES STIX_DEFAULT_IMAGE_CAPTURE_BACKEND ${backend})
endfunction()

function(stix_default_image_capture_status targetName status)
	set_target_properties(${targetName} PROPERTIES STIX_DEFAULT_IMAGE_CAPTURE_STATUS ${status})
endfunction()


# Main integration

set(REFLECTION_COMMAND python "${CMAKE_CURRENT_LIST_DIR}/rttigen.py")
function(stix_generate_reflection targetName rootDir)
	# Resolve optional properties
	get_target_property(TEMPLATE_FILE ${targetName} STIX_TEMPLATE_FILE)
	if (TEMPLATE_FILE STREQUAL "TEMPLATE_FILE-NOTFOUND")
		set(TEMPLATE_ARG )
	else()
		set(TEMPLATE_ARG --template ${TEMPLATE_FILE})
	endif()
	get_target_property(DEFAULT_IMAGE_CAPTURE_BACKEND ${targetName} STIX_DEFAULT_IMAGE_CAPTURE_BACKEND)
	if (DEFAULT_IMAGE_CAPTURE_BACKEND STREQUAL "DEFAULT_IMAGE_CAPTURE_BACKEND-NOTFOUND")
		set(DEFAULT_IMAGE_CAPTURE_BACKEND_ARG )
	else()
		set(DEFAULT_IMAGE_CAPTURE_BACKEND_ARG --default-image-capture-backend ${DEFAULT_IMAGE_CAPTURE_BACKEND})
	endif()
	get_target_property(DEFAULT_IMAGE_CAPTURE_STATUS ${targetName} STIX_DEFAULT_IMAGE_CAPTURE_STATUS)
	if (DEFAULT_IMAGE_CAPTURE_STATUS STREQUAL "DEFAULT_IMAGE_CAPTURE_STATUS-NOTFOUND")
		set(DEFAULT_IMAGE_CAPTURE_STATUS_ARG )
	else()
		set(DEFAULT_IMAGE_CAPTURE_STATUS_ARG --default-image-capture-status ${DEFAULT_IMAGE_CAPTURE_STATUS})
	endif()

	# Add rootDir to STIX_ROOT_DIRS (allows more complex directory structure)
	get_target_property(ROOT_DIRS ${targetName} STIX_ROOT_DIRS)
	if (ROOT_DIRS STREQUAL "ROOT_DIRS-NOTFOUND")
		set(ROOT_DIRS ${rootDir})
	else()
		list(APPEND ROOT_DIRS ${rootDir})
	endif()
	set_property(TARGET ${targetName} PROPERTY STIX_ROOT_DIRS ${ROOT_DIRS})

	# Add RTTI generator target if none exists
	if (NOT TARGET ${targetName}_RTTI)
		get_target_property(OUTPUT_FILE_DIR ${targetName} BINARY_DIR)
		set(OUTPUT_FILE "${OUTPUT_FILE_DIR}/generated-rtti/${targetName}_rtti.generated.cpp")
		set(CACHE_FILE "${OUTPUT_FILE_DIR}/generated-rtti/${targetName}.rtticache")

		if (STIX_RTTIGEN_PROFILING)
			set(PROFILING_ARGS --task-profiling --call-profiling)
		else()
			set(PROFILING_ARGS )
		endif()

		add_custom_target(
			${targetName}_RTTI
			BYPRODUCTS "${OUTPUT_FILE}" "${CACHE_FILE}"
 			COMMAND ${REFLECTION_COMMAND} --target $<TARGET_PROPERTY:${targetName},STIX_ROOT_DIRS>
										--output "${OUTPUT_FILE}"
										--cache "${CACHE_FILE}"
 										--include $<TARGET_PROPERTY:${targetName},INCLUDE_DIRECTORIES>
 										--define $<TARGET_PROPERTY:${targetName},COMPILE_DEFINITIONS>
 										${TEMPLATE_ARG}
										${DEFAULT_IMAGE_CAPTURE_BACKEND_ARG}
										${DEFAULT_IMAGE_CAPTURE_STATUS_ARG}
										${PROFILING_ARGS}
 										-- $<TARGET_PROPERTY:${targetName},COMPILE_OPTIONS> $<TARGET_PROPERTY:${targetName},COMPILE_FLAGS> ${COMPILE_OPTIONS} ${COMPILE_FLAGS}
			VERBATIM
			USES_TERMINAL
 			COMMENT "Generating embedded RTTI for ${targetName}"
 		)

		add_dependencies(${targetName} ${targetName}_RTTI)

		target_sources(${targetName} PRIVATE ${OUTPUT_FILE})
	endif()
endfunction()
