# set cmake minimum required version
cmake_minimum_required(VERSION 3.12)

# set cmake global features
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set project name
project(terrainGenerator)

# set compiler options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# MP build for MSVC
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")
endif()

# set project constants
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}) # project dir
set(BINARIES_DIR ${PROJECT_ROOT}/build) # binaries dir

# turn on use of MFC library
add_definitions(-D UNICODE)
add_definitions(-D _UNICODE)
set(CMAKE_MFC_FLAG 2)

# project declaration
file(GLOB_RECURSE SOURCES "src/*.c*" "src/*.h*")
add_executable(terrainGenerator ${SOURCES})

# set include directories
target_include_directories(terrainGenerator PUBLIC "src/" "resources/include/")

# set linkers
link_directories(terrainGenerator "resources/lib/")
file(GLOB_RECURSE LIBS  "resources/lib/*.lib")
target_link_libraries(terrainGenerator ${LIBS})

# Cpp20
target_compile_features(terrainGenerator PUBLIC cxx_std_20)

# set build properties
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_BUILD_TYPE "Release")