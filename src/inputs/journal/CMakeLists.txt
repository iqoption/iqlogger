add_library(${PROJECT_NAME}_input_journal Input.cpp JournalInput.cpp JournalReader.cpp CheckConfig.cpp)

target_link_libraries(
        ${PROJECT_NAME}_input_journal
        PUBLIC systemd
)
