# Copyright (c) 2015, J2 Innovations
# Licensed under the Academic Free License version 3.0
# History:
#   29 Aug 2014  Radu Racariu<radur@2inn.com> created.
#
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall /EHsc -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
    set( STATIC_LIBRARY_FLAGS "${STATIC_LIBRARY_FLAGS} /LTCG")
else (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif(MSVC)


set_property( GLOBAL PROPERTY USE_FOLDERS ON)

# Include files
include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB all_headers "${PROJECT_SOURCE_DIR}/include/*.h*")
source_group("headers" FILES ${all_headers}) 

# Boost Includes
find_package(Boost 1.40)


# Source files
FILE(GLOB src ${PROJECT_SOURCE_DIR}/src/*.cpp)
source_group(source FILES ${src})
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_library(haystack++ ${src} ${all_headers} ${Boost_INCLUDE_DIRS})
endif()