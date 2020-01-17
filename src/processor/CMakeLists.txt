find_library(
        V8_LIBRARY v8_monolith
)

add_library(${PROJECT_NAME}_processor Processor.cpp Engine.cpp)

target_link_libraries(
        ${PROJECT_NAME}_processor
        PUBLIC ${V8_LIBRARY}
)