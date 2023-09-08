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

	#set(OUTPUT_FILE "${rootDir}/src/rtti.generated.cpp")
	#set(OUTPUT_FILE $<TARGET_PROPERTY:${targetName},BINARY_DIR>/generated/${targetName}_rtti.generated.cpp)
	get_target_property(OUTPUT_FILE_DIR ${targetName} BINARY_DIR)
	set(OUTPUT_FILE "${OUTPUT_FILE_DIR}/generated/${targetName}_rtti.generated.cpp")

	add_custom_target(
		${targetName}_RTTI
		BYPRODUCTS "${OUTPUT_FILE}"
 		COMMAND ${REFLECTION_COMMAND} --target "${rootDir}"
									--output ${OUTPUT_FILE}
 									--include $<TARGET_PROPERTY:${targetName},INCLUDE_DIRECTORIES>
 									--define $<TARGET_PROPERTY:${targetName},COMPILE_DEFINITIONS>
 									${TEMPLATE_ARG}
 									-- $<TARGET_PROPERTY:${targetName},COMPILE_OPTIONS> $<TARGET_PROPERTY:${targetName},COMPILE_FLAGS> ${COMPILE_OPTIONS} ${COMPILE_FLAGS}
		VERBATIM
 		COMMENT "Generating embedded RTTI for ${targetName}"
 	)

	add_dependencies(${targetName} ${targetName}_RTTI)

	target_sources(${targetName} PRIVATE ${OUTPUT_FILE})
endfunction()
