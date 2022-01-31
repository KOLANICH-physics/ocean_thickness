pkg_check_modules(GLOG REQUIRED "libglog")

buildAndPackageLib("tdigest"
	COMPONENT "tdigest"
	TYPE INTERFACE
	DESCRIPTION "The lib implementing tdigest algo, https://github.com/derrickburns/tdigest"
	PUBLIC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE_INCLUDES ${GLOG_INCLUDE_DIRS}
	PUBLIC_LIBS ${GLOG_LIBRARIES}
)
