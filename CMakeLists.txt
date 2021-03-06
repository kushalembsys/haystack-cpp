# Copyright (c) 2015, J2 Innovations
# History:
#   29 Aug 2014  Radu Racariu<radur@2inn.com> created.
#

cmake_minimum_required (VERSION 2.8)
project (Haystack-cpp)
SET (APPLICATION_NAME "Haystack C++ Kit")

# The version number.
SET (APPLICATION_VERSION_MAJOR 1)
SET (APPLICATION_VERSION_MINOR 0)
SET (APPLICATION_VENDOR_ID "com.j2inn")
SET (APPLICATION_VENDOR_NAME "J2 Innovations")
SET (APPLICATION_VENDOR_URL "http://www.j2inn.com/")


option(BUILD_SERVER "Build the http-server" OFF)
option(BUILD_CLIENT "Build the http-client" OFF)
option(BUILD_TESTING "Build the tests" OFF)


FIND_PACKAGE(Boost)
if(${Boost_FOUND})
	ADD_SUBDIRECTORY(src)
	if(BUILD_TESTING)
	   ADD_SUBDIRECTORY(tests)
	endif(BUILD_TESTING)
endif()

SET(Poco_DIR ${PROJECT_SOURCE_DIR})
FIND_PACKAGE(Poco)
IF(${Poco_FOUND} AND ${Boost_FOUND})

    if(BUILD_SERVER)
	   ADD_SUBDIRECTORY(http_server)
	endif(BUILD_SERVER)
	
    if(BUILD_CLIENT)
	   ADD_SUBDIRECTORY(http_client)
    endif(BUILD_CLIENT)
	
ENDIF()