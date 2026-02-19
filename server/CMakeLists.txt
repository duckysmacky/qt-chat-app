find_package(Qt6 REQUIRED COMPONENTS Sql)

qt_add_executable(server_app
    src/main.cpp
	src/TcpServer.cpp
	src/TcpServer.h
	src/Database.cpp
	src/Database.h
)

target_link_libraries(server_app PRIVATE
    shared
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS server_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
