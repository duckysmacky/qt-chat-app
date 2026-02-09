qt_add_executable(server_app
    src/main.cpp
)

target_link_libraries(server_app PRIVATE
    shared
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)
