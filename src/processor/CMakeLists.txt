file(GLOB_RECURSE PROCESSOR_SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME}_processor ${PROCESSOR_SOURCE_FILES})

if (APPLE)
    find_library(
            V8_LIBRARY v8
    )
    find_library(
            V8_LIBRARY_PLATFORM v8_libplatform
    )
    target_link_libraries(
            ${PROJECT_NAME}_processor
            PUBLIC ${V8_LIBRARY} ${V8_LIBRARY_PLATFORM}
    )
else ()
    find_library(
            V8_LIBRARY v8_monolith
    )
    target_link_libraries(
            ${PROJECT_NAME}_processor
            PUBLIC ${V8_LIBRARY}
    )
endif ()


