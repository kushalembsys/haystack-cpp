# Copyright (c) 2015, J2 Innovations
# Licensed under the Academic Free License version 3.0
# History:
#   29 Aug 2014  Radu Racariu<radur@2inn.com> created.
#   06 January 2016 Maurus Frey<maurus.frey@siemens.com> adapated. 

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall /EHsc -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
    set( STATIC_LIBRARY_FLAGS "${STATIC_LIBRARY_FLAGS} /LTCG")
else (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif(MSVC)

set_property( GLOBAL PROPERTY USE_FOLDERS ON)

# Define installation directories.
set(INSTALL_LIB_DIR lib)
set(INSTALL_BIN_DIR bin)
set(INSTALL_INCLUDE_DIR include)
set(INSTALL_TEST_BIN_DIR bin)


# Include and public header files
include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB all_headers "${PROJECT_SOURCE_DIR}/include/*.h*")
source_group("headers" FILES ${all_headers}) 

# Source files
FILE(GLOB src ${PROJECT_SOURCE_DIR}/src/*.cpp)
source_group("source" FILES ${src})


# Building the library
######################################################################################################
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_library(haystack++  STATIC 
        ${src} 
        ${all_headers} 
        ${Boost_INCLUDE_DIRS}
        )
        elseif(Boost_FOUND)
          message( SEND_ERROR "GAGU" )
endif()



# Install public headers
set_target_properties(haystack++ PROPERTIES PUBLIC_HEADER "${all_headers}")


# Install
######################################################################################################
install(TARGETS haystack++
    LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
    PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}/haystack++ COMPONENT dev
    )
