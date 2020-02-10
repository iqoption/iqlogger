cmake_minimum_required(VERSION 3.7)
project(iqlogger)

set(IQLOGGER_INPUT_MODULES gelf tail journal json dummy)
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
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(DEFINED ENV{IQLOGGER_STATIC_GLIBC})
    set(IQLOGGER_STATIC_GLIBC ON)
endif()

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
    set(BUILD_COMMIT "$ENV{CI_COMMIT_SHA} ($ENV{CI_COMMIT_MESSAGE})")
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

if (DEFINED ENV{CI_USER})
    set(BUILD_USER "$ENV{CI_USER}")
else ()
    set(BUILD_USER "undefined")
endif ()

message("Build date: ${BUILD_DATE_TIME}")
message("Build user: ${BUILD_USER}")

configure_file (
        "${PROJECT_SOURCE_DIR}/src/config/Config.h.in"
        "${PROJECT_BINARY_DIR}/src/config/Config.h"
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

add_executable(${PROJECT_NAME} src/main.cpp
        src/config/Config.cpp
        src/config/core/Core.cpp
        src/config/core/QueueOverflowStrategy.cpp
        src/config/ConfigManager.cpp
        src/config/input/InputConfig.cpp
        src/config/output/OutputConfig.cpp
        src/config/logger/LoggerConfig.cpp
        src/config/processor/ProcessorConfig.cpp
        src/config/stats/StatsConfig.cpp
        src/MessageQueue.cpp
        src/inputs/InputInterface.cpp
        src/outputs/OutputInterface.cpp
        src/inputs/Input.cpp
        src/inputs/IOInput.cpp
        src/InputServer.cpp
        src/OutputServer.cpp
        src/outputs/Output.cpp
        src/core/Log.cpp
        src/core/Base.cpp
        src/metrics/MetricsController.cpp
        src/stats/StatsExportServer.cpp
        src/IQLogger.cpp
        src/MainMessageQueue.cpp
        src/formats/gelf/GelfMessage.cpp
        )

set(IQLOGGER_LIBS "${IQLOGGER_INPUT_LIBRARIES}${IQLOGGER_OUTPUT_LIBRARIES}${IQLOGGER_STATS_LIBRARIES};${Boost_LIBRARIES};${CPP_FILESYSTEM_LIB};tbb;pthread;z;")

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




