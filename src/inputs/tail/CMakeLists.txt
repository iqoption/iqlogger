add_library(${PROJECT_NAME}_input_tail Input.cpp CheckConfig.cpp TailInput.cpp TailMonitor.cpp InotifyServer.cpp Event.cpp SavePositionServer.cpp)

if (APPLE)
    find_library(
            INOTIFY_LIBRARY inotify
    )
    target_link_libraries(
            ${PROJECT_NAME}_input_tail
            PUBLIC ${INOTIFY_LIBRARY}
    )
endif ()