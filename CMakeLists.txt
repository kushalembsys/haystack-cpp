# Copyright (c) 2014, J2 Innovations
# History:
#   29 Aug 2014  Radu Racariu<radur@2inn.com> created.
#

cmake_minimum_required (VERSION 2.8)
project (Haystack-cpp)
SET (APPLICATION_NAME "Haystack C++ Library")

# The version number.
SET (APPLICATION_VERSION_MAJOR 1)
SET (APPLICATION_VERSION_MINOR 0)
SET (APPLICATION_VENDOR_ID "com.j2inn")
SET (APPLICATION_VENDOR_NAME "J2 Innovations")
SET (APPLICATION_VENDOR_URL "j2innvoations.com")

FIND_PACKAGE(Boost)
if(Boost_FOUND)
	ADD_SUBDIRECTORY(src)
	ADD_SUBDIRECTORY(tests)
endif()

SET(Poco_DIR ${PROJECT_SOURCE_DIR})
FIND_PACKAGE(Poco)
IF(${Poco_FOUND})
	ADD_SUBDIRECTORY(http_server)
ENDIF(${Poco_FOUND})