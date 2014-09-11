# Copyright (c) 2014, J2 Innovations
# History:
#   29 Aug 2014  Radu Racariu<radur@2inn.com> created.
#

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

# Include files
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/tests/include)
FILE(GLOB all_headers "${PROJECT_SOURCE_DIR}/include/*.h*")
# Source files
FILE(GLOB test_src ${PROJECT_SOURCE_DIR}/tests/*.cpp)
source_group(tests FILES ${test_src})


# Boost Includes
find_package(Boost 1.40)

if(Boost_FOUND)
    enable_testing()
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(test_app ${test_src})
    target_link_libraries(test_app haystack++)
    if (MSVC)
        set_target_properties(test_app PROPERTIES COMPILE_FLAGS "/Ycheaders.hpp")
        set_source_files_properties(headers.cpp PROPERTIES COMPILE_FLAGS "/Ycheaders.hpp")
    endif(MSVC)
endif()
