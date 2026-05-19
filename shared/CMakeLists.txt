add_library(shared_lib STATIC
    src/util.cpp
    src/util.h
    src/crypto.cpp
    src/crypto.h
    src/Packet.cpp
    src/Packet.h
    src/Message.cpp
    src/Message.h
    src/PacketFactory.cpp
    src/PacketFactory.h
    src/dto/AuthInfo.h
    src/dto/AuthInfo.cpp
        src/OperationResult.h
        src/OperationResult.cpp
        src/dto/ProfileInfo.h
        src/dto/ProfileInfo.cpp
        src/dto/ProfileUpdateInfo.h
        src/dto/ProfileUpdateInfo.cpp
        src/dto/PublicUserInfo.h
        src/dto/PublicUserInfo.cpp
        src/dto/UserInfoRequest.h
        src/dto/UserInfoRequest.cpp
        src/dto/SessionInfo.h
        src/dto/SessionInfo.cpp
        src/dto/ChatInfo.h
        src/dto/ChatInfo.cpp
        src/dto/ChatsInfo.h
        src/dto/ChatsInfo.cpp
        src/dto/CreateChatInfo.h
        src/dto/CreateChatInfo.cpp
        src/KeyStore.h
        src/KeyStore.cpp
)

target_include_directories(shared_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(shared_lib PUBLIC
    Qt6::Core
    Qt6::Network
    Qt-Secret
)

if(BUILD_TESTING)
    add_module_tests(shared
        shared_lib
    )
endif()
