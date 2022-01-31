buildAndPackageLib("p2"
	COMPONENT "p2"
	DESCRIPTION "The lib implementing p^2 algo, https://github.com/absmall/p2"
	PUBLIC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/p2
)
get_target_property(P2_SOURCES p2 SOURCES)
list(REMOVE_ITEM P2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/p2/main.cc")
set_target_properties(p2 PROPERTIES SOURCES ${P2_SOURCES})
