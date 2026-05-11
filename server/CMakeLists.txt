find_package(Qt6 REQUIRED COMPONENTS Sql)

add_library(server_lib STATIC
	src/Server.cpp
	src/Server.h
	src/Database.cpp
	src/Database.h
	src/ConsoleReader.cpp
	src/ConsoleReader.h
	src/ClientConnection.h
	src/ClientConnection.cpp
	src/middleware.h
	src/middleware.cpp
	src/model/Chat.cpp
	src/model/Chat.h
	src/model/ChatMember.cpp
	src/model/ChatMember.h
	src/model/Content.cpp
	src/model/Content.h
	src/model/DbMessage.cpp
	src/model/DbMessage.h
	src/model/User.cpp
	src/model/User.h
	src/model/UserStats.cpp
	src/model/UserStats.h
)

target_include_directories(server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(server_lib PUBLIC
    shared_lib
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

qt_add_executable(server_app
    src/main.cpp
)

target_link_libraries(server_app PRIVATE
    server_lib
)

include(GNUInstallDirs)
install(TARGETS server_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    enable_qt_windows_deploy(server_app)
endif()

add_module_tests(server
    server_lib
)