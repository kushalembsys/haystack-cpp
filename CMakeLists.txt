# J2 Innovations. Copyright (C) 2014.

cmake_minimum_required (VERSION 2.8)
project (Haystack-cpp)
SET (APPLICATION_NAME "Haystack C++ Library")

# The version number.
SET (APPLICATION_VERSION_MAJOR 1)
SET (APPLICATION_VERSION_MINOR 0)
SET (APPLICATION_VENDOR_ID "com.j2inn")
SET (APPLICATION_VENDOR_NAME "J2 Innovations")
SET (APPLICATION_VENDOR_URL "j2innvoations.com")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)