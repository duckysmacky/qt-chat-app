add_library(shared STATIC
)

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(shared PUBLIC
    Qt6::Core
    Qt6::Network
)