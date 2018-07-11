# FindALURE, loosely based on FindOpenAL.cmake
# Defines ${ALURE_INCLUDE_DIR} and ${ALURE_LIBRARY}

find_path(ALURE_INCLUDE_DIR alure.h
  HINTS
    ENV ALUREDIR
  PATH_SUFFIXES include
  PATHS
    /usr/include
    /usr/include/AL
    /usr/local/include
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_ALURE_ARCH_DIR lib/Win64)
else()
  set(_ALURE_ARCH_DIR lib/Win32)
endif()

find_library(ALURE_LIBRARY
  NAMES ALURE ALURE32 alure alure32
  HINTS
    ENV ALUREDIR
  PATH_SUFFIXES libx32 lib64 lib libs64 libs ${_ALURE_ARCH_DIR}
  PATHS
    /usr/lib
    /usr/local/lib
)

unset(_ALURE_ARCH_DIR)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALURE DEFAULT_MSG ALURE_LIBRARY ALURE_INCLUDE_DIR)
