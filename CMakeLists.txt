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

configure_file (
        "${PROJECT_SOURCE_DIR}/src/config/Config.h.in"
        "${PROJECT_BINARY_DIR}/src/config/Config.h"
)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)

find_package(Boost COMPONENTS system iostreams program_options REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL "Boost not found!")
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

set(IQLOGGER_LIBS "${IQLOGGER_INPUT_LIBRARIES}${IQLOGGER_OUTPUT_LIBRARIES}${IQLOGGER_STATS_LIBRARIES};${Boost_LIBRARIES};tbb;pthread;z;stdc++fs")

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




