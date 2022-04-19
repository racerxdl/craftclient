cmake_minimum_required(VERSION 3.10)

# set the project name
project(Craftlient CXX)


# specify the C++ standard
cmake_policy(SET CMP0067 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_REQUIRED_FLAGS "-std=c++17")
# set(CMAKE_CXX_FLAGS "-fPIC -std=c++11") # -O2
#set(CMAKE_C_FLAGS "-fPIC -O2")
set(BUILD_SHARED_LIBS ON)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(conan.cmake)

conan_cmake_configure(REQUIRES glfw/3.3.6 libpng/1.6.37
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

# Find source files
file(GLOB SOURCES src/*.cpp)
# set(SOURCES ${SOURCES} ${CRAFTBLOCK_SOURCES})


find_package(PkgConfig REQUIRED)
find_package(PNG)

# Dependencies

add_subdirectory(libs/craftblock)

set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

add_subdirectory(libs/nanogui)
add_definitions(${NANOGUI_EXTRA_DEFS})
include_directories(${NANOGUI_EXTRA_INCS})

set_property(TARGET glfw glfw_objects nanogui CraftBlock PROPERTY FOLDER "dependencies")

# Include header files
include_directories(include)
include_directories(libs/craftblock/include)
include_directories(libs/linmath/include)
include_directories(libs/glad/include)
include_directories(libs/nanogui/include)

add_executable(${PROJECT_NAME} ${SOURCES})
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Link deps
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} nanogui)
target_link_libraries(${PROJECT_NAME} PNG::PNG)
target_link_libraries(${PROJECT_NAME} ${NANOGUI_EXTRA_LIBS})
target_link_libraries(${PROJECT_NAME} CraftBlock)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MAJOR 1)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MINOR 0)
set_target_properties(${PROJECT_NAME} PROPERTIES DESCRIPTION "Craft Client")

# Install library headers
file(GLOB HEADERS include/*.h)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})