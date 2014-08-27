#set(CMAKE_CXX_FLAGS "-Wall")
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

# Include files
include_directories(${PROJECT_SOURCE_DIR}/include)
FILE(GLOB all_headers "${PROJECT_SOURCE_DIR}/include/*.h*")
# Source files
FILE(GLOB test_src ${PROJECT_SOURCE_DIR}/tests/*.cpp)
source_group(tests FILES ${test_src})


enable_testing()
add_executable(test_app ${test_src})
target_link_libraries(test_app libhaystack++)

if (MSVC)
   set_target_properties(test_app PROPERTIES COMPILE_FLAGS "/Ycheaders.hpp")
   set_source_files_properties(headers.cpp PROPERTIES COMPILE_FLAGS "/Ycheaders.hpp")
endif(MSVC)