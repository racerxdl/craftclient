cmake_minimum_required(VERSION 3.10)

# set the project name
project(ProtoRock CXX)
set(ProtoRock_VERSION_MAJOR 1)
set(ProtoRock_VERSION_MINOR 0)

# specify the C++ standard
cmake_policy(SET CMP0067 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_REQUIRED_FLAGS "-std=c++17")
# set(CMAKE_REQUIRED_FLAGS "-std=c++17")
# set(CMAKE_CXX_FLAGS "-fPIC -O2")
#set(CMAKE_CXX_FLAGS "-fPIC -O2")
#set(CMAKE_C_FLAGS "-fPIC -O2")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#add_definitions("-std=c++17")

include(conan.cmake)

conan_cmake_configure(REQUIRES asio/1.22.1 cpr/1.8.1 tiny-aes-c/1.0.0 libdeflate/1.10 libuuid/1.0.3 libcurl/7.80.0 jwt-cpp/0.6.0 picojson/1.3.0 openssl/1.1.1m fmt/8.0.0 jsoncpp/1.9.5 date/3.0.1
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})


file(GLOB_RECURSE HEADERS include/*.h)

# Dependencies
find_package(asio)
find_package(libuuid)
find_package(picojson)
find_package(jwt-cpp)
find_package(fmt)
find_package(jsoncpp)
find_package(date)
find_package(cpr)
find_package(libdeflate)
find_package(tiny-aes-c)

# Find source files
file(GLOB SOURCES src/*.cpp src/common/*.cpp src/minecraft/packet/*.cpp src/minecraft/*.cpp src/raknet/*.cpp src/protocol/*.cpp src/oauth2/*.cpp)
file(GLOB TEST_SRC testprog/*.cpp)

# Include header files
include_directories(include)
include_directories(include/common)
include_directories(include/oauth2)
include_directories(include/raknet)
include_directories(include/protocol)
include_directories(include/minecraft)
include_directories(../craftblock/include)
include_directories(${picojson_INCLUDE_DIRS})
include_directories(${date_INCLUDE_DIRS})
include_directories(${libuuid_INCLUDE_DIRS})

add_definitions(${jwt-cpp_DEFINITIONS})

set(LINKED_LIBS asio::asio libuuid::libuuid tiny-aes-c::tiny-aes-c libdeflate::libdeflate jwt-cpp::jwt-cpp jsoncpp::jsoncpp fmt::fmt cpr::cpr jsoncpp)

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