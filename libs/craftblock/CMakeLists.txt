cmake_minimum_required(VERSION 3.10)

# set the project name
project(CraftBlock CXX)
set(CraftBlock_VERSION_MAJOR 1)
set(CraftBlock_VERSION_MINOR 0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-fPIC -O2")
set(CMAKE_C_FLAGS "-fPIC -O2")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS include/*.h)

# Find source files
file(GLOB SOURCES src/*.cpp)
file(GLOB BLOCK_SOURCES src/blocks/*.cpp)
set(SOURCES ${SOURCES} ${BLOCK_SOURCES})

# Include header files
include_directories(include)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})


set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CraftBlock_VERSION_MAJOR}_${CraftBlock_VERSION_MINOR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MAJOR ${CraftBlock_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MINOR ${CraftBlock_VERSION_MINOR})
set_target_properties(${PROJECT_NAME} PROPERTIES DESCRIPTION "CraftBlock")

add_definitions( -DCRAFTBLOCK_VERSION=${CraftBlock_VERSION_MAJOR}.${CraftBlock_VERSION_MINOR} )

# Install library headers
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin)