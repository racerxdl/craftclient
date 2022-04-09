cmake_minimum_required(VERSION 3.10)

# set the project name
project(Craftlient CXX)

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
add_definitions("-std=c++11") # Conan

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
# set(SOURCES
#     src/glad.cpp
#     src/gui.cpp
#     src/main.cpp
#     src/matrix.cpp
#     src/mesh.cpp
#     src/object.cpp
#     src/shader.cpp
#     src/texture.cpp
#     src/vectors.cpp
#     src/image.cpp
# )

find_package(PkgConfig REQUIRED)
find_package(PNG)

# Dependencies

set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

#find_package(glfw3)
add_subdirectory(libs/nanogui)
set_property(TARGET glfw glfw_objects nanogui PROPERTY FOLDER "dependencies")
add_definitions(${NANOGUI_EXTRA_DEFS})
include_directories(${NANOGUI_EXTRA_INCS})

# Include header files
include_directories(include)
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

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MAJOR 1)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION_MINOR 0)
set_target_properties(${PROJECT_NAME} PROPERTIES DESCRIPTION "Craft Client")

# Install library headers
file(GLOB HEADERS include/*.h)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})