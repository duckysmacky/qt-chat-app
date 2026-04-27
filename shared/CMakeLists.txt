add_library(shared STATIC
    src/util.cpp
    src/util.h
    src/Packet.cpp
    src/Packet.h
    src/Message.cpp
    src/Message.h
    src/PacketFactory.cpp
    src/PacketFactory.h
    src/dto/AuthInfo.h
    src/dto/AuthInfo.cpp
        src/Result.h
        src/Result.cpp
        src/dto/ProfileInfo.h
        src/dto/ProfileInfo.cpp
        src/dto/ProfileUpdateInfo.h
        src/dto/ProfileUpdateInfo.cpp
        src/dto/PublicUserInfo.h
        src/dto/PublicUserInfo.cpp
)

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(shared PUBLIC
    Qt6::Core
    Qt6::Network
)
