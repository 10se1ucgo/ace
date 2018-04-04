cmake_minimum_required(VERSION 3.6)
project(ace)

set(CMAKE_SOURCE_DIR src)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (MSVC)
    # TOTALLY not from iceball
	set(CMAKE_PREFIX_PATH "ext/sdl2;ext/enet;ext/zlib;ext/openal;ext/freetype;ext/glm;ext/alure" CACHE PATH "" FORCE)
	set_source_files_properties(filename.c PROPERTIES LANGUAGE CXX)
    # without this i get errors when linkiing OpenAL `module unsafe for SAFESEH` image.
    # the fix would be to compile OpenAL myself? but LAZY so ill do it later and leave this crap here 
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif (MSVC)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(ENet REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ALURE REQUIRED)
find_package(FreeType REQUIRED)
find_package(GLM REQUIRED)

include_directories(include ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${ENet_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${ALURE_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_FILES src/*.c src/*.cpp src/*.cc)
file(GLOB_RECURSE INC_FILES include/*.h include/*.hpp include/*.hh)

add_executable(ace ${SRC_FILES} ${INC_FILES})
add_subdirectory(ext/fmt)
target_link_libraries(ace ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${ENet_LIBRARIES} ${ZLIB_LIBRARIES} ${OPENAL_LIBRARY} ${ALURE_LIBRARY} ${FREETYPE_LIBRARIES} fmt::fmt)