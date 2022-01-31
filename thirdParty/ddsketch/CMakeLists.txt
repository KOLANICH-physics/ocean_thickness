buildAndPackageLib("ddsketch"
	COMPONENT "ddsketch"
	TYPE INTERFACE
	DESCRIPTION "The lib implementing tdigest algo, https://github.com/tudor1805/sketches-cpp"
	PUBLIC_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/ddsketch/include"
)
