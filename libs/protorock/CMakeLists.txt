cmake_minimum_required(VERSION 3.10)

# set the project name
project(ProtoRock CXX)
set(ProtoRock_VERSION_MAJOR 1)
set(ProtoRock_VERSION_MINOR 0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_FLAGS "-fPIC -O2")
#set(CMAKE_C_FLAGS "-fPIC -O2")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(conan.cmake)

conan_cmake_configure(REQUIRES jwt-cpp/0.6.0 picojson/1.3.0 cppserver/1.0.1.0 openssl/1.1.1m
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})


file(GLOB_RECURSE HEADERS include/*.h)

# Dependencies
find_package(picojson)
find_package(jwt-cpp)
find_package(cppserver)

# Find source files
file(GLOB SOURCES src/*.cpp src/raknet/*.cpp)
file(GLOB TEST_SRC testprog/*.cpp)

# Include header files
include_directories(include include/raknet )
include_directories(${picojson_INCLUDE_DIRS})
include_directories(${jwt-cpp_INCLUDE_DIRS})
include_directories(${asio_INCLUDE_DIRS})
include_directories(${cppcommon_INCLUDE_DIRS})
include_directories(${cppserver_INCLUDE_DIRS})

add_definitions(${jwt-cpp_DEFINITIONS})
add_definitions(${cppserver_DEFINITIONS})

set(LINKED_LIBS jwt-cpp::jwt-cpp cppserver::cppserver)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
add_executable(${PROJECT_NAME}_test ${HEADERS} ${SOURCES} ${TEST_SRC})

target_link_libraries(${PROJECT_NAME} ${LINKED_LIBS})
target_link_libraries(${PROJECT_NAME}_test ${LINKED_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${ProtoRock_VERSION_MAJOR}_${ProtoRock_VERSION_MINOR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MAJOR ${ProtoRock_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MINOR ${ProtoRock_VERSION_MINOR})
set_target_properties(${PROJECT_NAME} PROPERTIES DESCRIPTION "ProtoRock")

add_definitions( -DProtoRock_VERSION=${ProtoRock_VERSION_MAJOR}.${ProtoRock_VERSION_MINOR} )

# Install library headers
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib
                                ARCHIVE DESTINATION lib
                                RUNTIME DESTINATION bin)