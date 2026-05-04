find_package(Qt6 REQUIRED COMPONENTS Qml Quick Network)

qt_add_executable(client_app
    src/main.cpp
    src/Client.cpp
    src/Client.h
    src/RequestManager.cpp
    src/RequestManager.h
    src/Chat.cpp
    src/Chat.h
    src/ChatMessage.cpp
    src/ChatMessage.h
    src/Hasher.cpp
    src/Hasher.h
	src/AccountManager.cpp
	src/AccountManager.h
	src/MessageSender.cpp
	src/MessageSender.h
	src/ChatManager.cpp
	src/ChatManager.h
)

qt_add_qml_module(client_app
    URI ChatApp
    VERSION 1.0
    QML_FILES
        src/ui/ChatBox.qml
        src/ui/ChatMessage.qml
        src/ui/ConnectionPanel.qml
        src/ui/ConnectionWindow.qml
        src/ui/ChatWindow.qml
        src/ui/ProfilePanel.qml
        src/ui/LoginPanel.qml
		src/ui/AccountWindow.qml
        src/ui/RegisterPanel.qml
        src/ui/StatusIndicator.qml
		src/ui/ChatSelectionPanel.qml
		src/ui/ChatCard.qml
)

target_link_libraries(client_app PRIVATE
    shared
    Qt6::Quick
    Qt6::Qml
    Qt6::Network
)

set_target_properties(client_app PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS client_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    enable_qt_windows_deploy(client_app QML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ui")
endif()
