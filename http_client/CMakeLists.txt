SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
   "Single output directory for building all executables.")

SET( TARGET_BASE_NAME "http_client" )
SET( LIB_NAME "${TARGET_BASE_NAME}Lib" )

#-----------------------------------------------------------------------------
# Let's use the highest warning level.
#-----------------------------------------------------------------------------
IF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
  # Use the highest warning level for visual studio.
  IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  ELSE(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  ENDIF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
  IF(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  ELSE(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  ENDIF(CMAKE_C_FLAGS MATCHES "/W[0-4]")
  # Disable deprecation warnings for standard C functions in VS2005 and later
  IF(CMAKE_COMPILER_2005)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  ENDIF(CMAKE_COMPILER_2005)

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
  set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

ENDIF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
IF(CMAKE_BUILD_TOOL MATCHES "make")
  IF(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  ENDIF(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
  IF(NOT CMAKE_C_FLAGS MATCHES "-Wall")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
  ENDIF(NOT CMAKE_C_FLAGS MATCHES "-Wall")
ENDIF(CMAKE_BUILD_TOOL MATCHES "make")

#SET(Poco_DIR ${PROJECT_SOURCE_DIR/http_server})

IF(${Poco_FOUND})
  INCLUDE_DIRECTORIES(${Poco_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Poco_LIBRARY_DIRS})
#ENDIF(WIN32)
ENDIF(${Poco_FOUND})

#-----------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/http_server/include)
include_directories(${PROJECT_SOURCE_DIR}/http_client/include)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

SET( EXE_NAME "${TARGET_BASE_NAME}" )
# Add any source files here.
FILE(GLOB EXE_SRCS ${PROJECT_SOURCE_DIR}/http_client/src/*.cpp)
# Add any include files here.
FILE(GLOB EXE_INCS  ${PROJECT_SOURCE_DIR}/http_client/include/*.h*)

# Source files
source_group(src FILES ${EXE_SRCS})
source_group(headers FILES ${EXE_INCS})

#-----------------------------------------------------------------------------
# The executable.
ADD_EXECUTABLE(${EXE_NAME} ${EXE_SRCS} ${EXE_INCS})

TARGET_LINK_LIBRARIES ( ${EXE_NAME}
  "optimized" PocoFoundation
  "debug" PocoFoundationd
  "optimized" PocoUtil
  "debug" PocoUtild
  "optimized" PocoNet
  "debug" PocoNetd
 )

 target_link_libraries(${EXE_NAME} haystack++)

