find_package(Qt6 REQUIRED COMPONENTS Sql)

qt_add_executable(server_app
    src/main.cpp
		src/TcpServer.cpp
		src/TcpServer.h
		src/Database.cpp
		src/Database.h
		src/ConsoleReader.cpp
		src/ConsoleReader.h
		src/middleware.h
		src/middleware.cpp
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

if(WIN32)
    enable_qt_windows_deploy(server_app)
endif()
