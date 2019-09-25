cmake_minimum_required(VERSION 3.6)
project(ace)
set(CMAKE_SOURCE_DIR src)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_PYBIND11 "Embed Python interpreter + generate C++ bindings with pybind11" OFF)

if (MSVC)
    # TOTALLY not from iceball
    set(CMAKE_PREFIX_PATH "ext/sdl2;ext/enet;ext/zlib;ext/openal;ext/freetype;ext/glm;ext/alure;ext/nlohmann;ext/libcurl" CACHE PATH "" FORCE)
    set_source_files_properties(filename.c PROPERTIES LANGUAGE CXX)
    # without this i get errors when linkiing OpenAL `module unsafe for SAFESEH` image.
    # the fix would be to compile OpenAL myself? but LAZY so ill do it later and leave this crap here 
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
    add_definitions(-DCURL_STATICLIB)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # If you are building C++14 with MSVC, you have snprintf.
    # pyerrors.h can stop breaking code with its macro thank you VERY much.
    add_definitions(-DHAVE_SNPRINTF=1)
endif (MSVC)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ENet REQUIRED)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(GLM REQUIRED)
find_package(ALURE REQUIRED)

find_path(JSON_INCLUDE_DIRS "nlohmann/json.hpp" HINTS "include")
if(JSON_INCLUDE_DIRS STREQUAL "JSON_INCLUDE_DIRS-NOTFOUND")
    message( FATAL_ERROR "Nlohmann JSON library not found" )
endif()

include_directories(include ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR}
                            ${ENet_INCLUDE_DIRS}
                            ${ZLIB_INCLUDE_DIRS}
                            ${OPENAL_INCLUDE_DIR} ${ALURE_INCLUDE_DIR}
                            ${FREETYPE_INCLUDE_DIRS}
                            ${GLM_INCLUDE_DIRS}
                            ${JSON_INCLUDE_DIRS}
                            ${CURL_INCLUDE_DIRS})

# This is bad and I should feel bad, I know
# Maybe one day I will rewrite this CMakeLists
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c src/*.cpp src/*.cc)
file(GLOB_RECURSE INC_FILES CONFIGURE_DEPENDS include/*.h include/*.hpp include/*.hh)

if(USE_PYBIND11)
    file(GLOB_RECURSE BINDING_FILES CONFIGURE_DEPENDS python_bindings/*.h python_bindings/*.cpp)
    include_directories(python_bindings)

    add_definitions(-DUSE_PYBIND11)
    add_subdirectory(ext/pybind11)  
endif(USE_PYBIND11)

add_subdirectory(ext/fmt)

add_executable(ace ${SRC_FILES} ${INC_FILES} ${BINDING_FILES})
target_link_libraries(ace ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES}
                          ${OPENGL_LIBRARIES} 
                          ${ENet_LIBRARIES}
                          ${ZLIB_LIBRARIES}
                          ${OPENAL_LIBRARY} ${ALURE_LIBRARY}
                          ${FREETYPE_LIBRARIES}
                          ${CURL_LIBRARIES}
                          ${CMAKE_DL_LIBS}
                          fmt::fmt)

if(USE_PYBIND11)
    target_link_libraries(ace pybind11::embed)
endif(USE_PYBIND11)
