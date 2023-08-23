cmake_minimum_required (VERSION 3.14)
set (CMAKE_CXX_STANDARD 17)

project("engine-reflection-tools")

set(REFLECTION_COMMAND python "${CMAKE_CURRENT_LIST_DIR}/rttigen.py")

function(generate_reflection targetName rootDir)
	#get_target_property(TARGET_SOURCE_DIR ${targetName} SOURCE_DIR)

	# Resolve optional properties
	get_target_property(RTTI_TEMPLATE_FILE ${targetName} RTTI_TEMPLATE_FILE)
	if (RTTI_TEMPLATE_FILE STREQUAL "RTTI_TEMPLATE_FILE-NOTFOUND")
		set(RTTI_TEMPLATE_ARG )
	else()
		set(RTTI_TEMPLATE_ARG --template ${RTTI_TEMPLATE_FILE})
	endif()

	add_custom_target(
		${targetName}_RTTI
		BYPRODUCTS "${rootDir}/src/rtti.generated.cpp"
		COMMAND ${REFLECTION_COMMAND} --target "${rootDir}"
									--include $<TARGET_PROPERTY:${targetName},INCLUDE_DIRECTORIES>
									--define $<TARGET_PROPERTY:${targetName},COMPILE_DEFINITIONS>
									${RTTI_TEMPLATE_ARG}
									-- $<TARGET_PROPERTY:${targetName},COMPILE_OPTIONS> $<TARGET_PROPERTY:${targetName},COMPILE_FLAGS> ${COMPILE_OPTIONS} ${COMPILE_FLAGS}
		COMMENT "Generating embedded RTTI for ${targetName}"
	)
	add_dependencies(${targetName} ${targetName}_RTTI)
endfunction()
