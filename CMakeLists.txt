cmake_minimum_required(VERSION 3.7)
project(iqlogger)

#set(IQLOGGER_INPUT_MODULES gelf tail journal json dummy)
set(IQLOGGER_INPUT_MODULES gelf tail json dummy)
set(IQLOGGER_OUTPUT_MODULES gelf log)
set(IQLOGGER_STATS_MODULES http telegraf)
set(IQLOGGER_WITH_PROCESSOR ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(DEFINED ENV{IQLOGGER_STATIC_GLIBC})
    set(IQLOGGER_STATIC_GLIBC ON)
endif()

#add_compile_definitions(BOOST_ASIO_NO_DEPRECATED)

file(READ version IQLOGGER_VERSION)

message(STATUS "IQLogger Input Modules: ${IQLOGGER_INPUT_MODULES}")
message(STATUS "IQLogger Output Modules: ${IQLOGGER_OUTPUT_MODULES}")

configure_file (
        "${PROJECT_SOURCE_DIR}/src/Version.h.in"
        "${PROJECT_BINARY_DIR}/src/Version.h"
)

if (DEFINED ENV{CI_PIPELINE_ID})
    set(BUILD_ID "$ENV{CI_PIPELINE_ID}")
else ()
    set(BUILD_ID "undefined")
endif ()
message("Build id: ${BUILD_ID}")

if (DEFINED ENV{CI_COMMIT_REF_NAME})
    set(BUILD_COMMIT "$ENV{CI_COMMIT_REF_NAME} ($ENV{CI_COMMIT_SHA})")
    set(GIT_VERSION "$ENV{CI_COMMIT_REF_NAME}")
else ()
    set(BUILD_COMMIT "undefined (undefined)")
    execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()
message("Build commit: ${BUILD_COMMIT}")
message("Git version: ${GIT_VERSION}")

string(TIMESTAMP BUILD_DATE_TIME "%Y-%m-%d %H:%M:%S UTC" UTC)

if (DEFINED ENV{GITLAB_USER_EMAIL})
    set(BUILD_USER "$ENV{GITLAB_USER_EMAIL}")
else ()
    set(BUILD_USER "undefined")
endif ()

message("Build date: ${BUILD_DATE_TIME}")
message("Build user: ${BUILD_USER}")


configure_file (
        "${PROJECT_SOURCE_DIR}/src/config/Config.h.in"
        "${PROJECT_BINARY_DIR}/src/config/Config.h"
)

configure_file (
        "${PROJECT_SOURCE_DIR}/src/Defines.h.in"
        "${PROJECT_BINARY_DIR}/src/Defines.h"
)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost COMPONENTS system iostreams program_options regex REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL "Boost not found!")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CPP_FILESYSTEM_LIB "c++fs")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CPP_FILESYSTEM_LIB "stdc++fs")
    set(EC_STATIC_GLIBC ON)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message(FATAL "Intel is not supported!")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(FATAL "MS Visual Studio is not supported!")
endif()

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories(${Boost_INCLUDE_DIRS})

include(cmake/input.CMakeLists.txt)
include(cmake/output.CMakeLists.txt)
include(cmake/stats.CMakeLists.txt)

file(GLOB IQLOGGER_SOURCE_FILES_ROOT "src/*.cpp")
file(GLOB_RECURSE IQLOGGER_SOURCE_FILES_CONFIG "src/config/*.cpp")
file(GLOB_RECURSE IQLOGGER_SOURCE_FILES_CORE "src/core/*.cpp")
file(GLOB_RECURSE IQLOGGER_SOURCE_FILES_FORMATS "src/formats/*.cpp")
file(GLOB IQLOGGER_SOURCE_FILES_INPUTS "src/inputs/*.cpp")
file(GLOB_RECURSE IQLOGGER_SOURCE_FILES_METRICS "src/metrics/*.cpp")
file(GLOB IQLOGGER_SOURCE_FILES_OUTPUTS "src/outputs/*.cpp")
file(GLOB_RECURSE IQLOGGER_SOURCE_FILES_STATS "src/stats/*.cpp")
file(GLOB_RECURSE IQLOGGER_SOURCE_FILES_UTILS "src/utils/*.cpp")

set(IQLOGGER_SOURCE_FILES "${IQLOGGER_SOURCE_FILES_ROOT};${IQLOGGER_SOURCE_FILES_CONFIG};${IQLOGGER_SOURCE_FILES_CORE};${IQLOGGER_SOURCE_FILES_FORMATS};${IQLOGGER_SOURCE_FILES_INPUTS};${IQLOGGER_SOURCE_FILES_METRICS};${IQLOGGER_SOURCE_FILES_OUTPUTS};${IQLOGGER_SOURCE_FILES_STATS};${IQLOGGER_SOURCE_FILES_UTILS}")

message(STATUS "IQLOGGER IQLOGGER_SOURCE_FILES: ${IQLOGGER_SOURCE_FILES}")

add_executable(${PROJECT_NAME} ${IQLOGGER_SOURCE_FILES})

set(IQLOGGER_LIBS "${IQLOGGER_INPUT_LIBRARIES}${IQLOGGER_OUTPUT_LIBRARIES}${IQLOGGER_STATS_LIBRARIES};${Boost_LIBRARIES};${CPP_FILESYSTEM_LIB};tbb;pthread;z")

if(IQLOGGER_WITH_PROCESSOR)
    message("Processor detected")
    add_subdirectory("src/processor")
    set(IQLOGGER_LIBS "${PROJECT_NAME}_processor${IQLOGGER_LIBS}")
endif(IQLOGGER_WITH_PROCESSOR)

target_link_libraries(${PROJECT_NAME} ${IQLOGGER_LIBS})

if(IQLOGGER_STATIC_GLIBC)
    message("Static linking with libgcc & libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "IQLOGGER_LIBS: ${IQLOGGER_LIBS}")
message(STATUS "IQLOGGER Version: ${IQLOGGER_VERSION}")




