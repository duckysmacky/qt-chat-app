add_library(shared STATIC
    src/Message.h
    src/Message.cpp
)

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(shared PUBLIC
    Qt6::Core
    Qt6::Network
)
