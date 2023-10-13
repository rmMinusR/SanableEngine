cmake_minimum_required (VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

project("engine-reflection-tools")

set(REFLECTION_COMMAND python "${CMAKE_CURRENT_LIST_DIR}/rttigen.py")

function(generate_reflection targetName rootDir)
	# Resolve optional properties
	get_target_property(TEMPLATE_FILE ${targetName} RTTI_TEMPLATE_FILE)
	if (TEMPLATE_FILE STREQUAL "TEMPLATE_FILE-NOTFOUND")
		set(TEMPLATE_ARG )
	else()
		set(TEMPLATE_ARG --template ${TEMPLATE_FILE})
	endif()

	# Add rootDir to RTTI_ROOT_DIRS (allows more complex directory structure)
	get_target_property(ROOT_DIRS ${targetName} RTTI_ROOT_DIRS)
	if (ROOT_DIRS STREQUAL "ROOT_DIRS-NOTFOUND")
		set(ROOT_DIRS ${rootDir})
	else()
		list(APPEND ROOT_DIRS ${rootDir})
	endif()
	set_property(TARGET ${targetName} PROPERTY RTTI_ROOT_DIRS ${ROOT_DIRS})

	# Add RTTI generator target if none exists
	if (NOT TARGET ${targetName}_RTTI)
		get_target_property(OUTPUT_FILE_DIR ${targetName} BINARY_DIR)
		set(OUTPUT_FILE "${OUTPUT_FILE_DIR}/generated-rtti/${targetName}_rtti.generated.cpp")
		set(CACHE_DIR "${OUTPUT_FILE_DIR}/generated-rtti/${targetName}.rtticache")

		add_custom_target(
			${targetName}_RTTI
			BYPRODUCTS "${OUTPUT_FILE}" "${CACHE_DIR}"
 			COMMAND ${REFLECTION_COMMAND} --target $<TARGET_PROPERTY:${targetName},RTTI_ROOT_DIRS>
										--output "${OUTPUT_FILE}"
										--cache "${CACHE_DIR}"
 										--include $<TARGET_PROPERTY:${targetName},INCLUDE_DIRECTORIES>
 										--define $<TARGET_PROPERTY:${targetName},COMPILE_DEFINITIONS>
 										${TEMPLATE_ARG}
 										-- $<TARGET_PROPERTY:${targetName},COMPILE_OPTIONS> $<TARGET_PROPERTY:${targetName},COMPILE_FLAGS> ${COMPILE_OPTIONS} ${COMPILE_FLAGS}
			VERBATIM
 			COMMENT "Generating embedded RTTI for ${targetName}"
 		)

		add_dependencies(${targetName} ${targetName}_RTTI)

		target_sources(${targetName} PRIVATE ${OUTPUT_FILE})
	endif()
endfunction()
