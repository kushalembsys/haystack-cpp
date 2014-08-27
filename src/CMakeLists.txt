if (MSVC)
	set(CMAKE_CXX_FLAGS "-Wall /EHsc -D_CRT_SECURE_NO_WARNINGS")
else (MSVC)
	set(CMAKE_CXX_FLAGS "-Wall")
endif(MSVC)

set_property( GLOBAL PROPERTY USE_FOLDERS ON)

# Include files
include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB all_headers "${PROJECT_SOURCE_DIR}/include/*.h*")
source_group("headers" FILES ${all_headers}) 
# Source files
FILE(GLOB src ${PROJECT_SOURCE_DIR}/src/*.cpp)
source_group(source FILES ${src})

add_library(libhaystack++ ${src} ${all_headers})