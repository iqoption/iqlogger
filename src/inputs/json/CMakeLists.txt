add_library(${PROJECT_NAME}_input_json
    CheckConfig.cpp
    udp/JsonInput.cpp
    tcp/JsonInput.cpp
    udp/Server.cpp
    tcp/Server.cpp
)