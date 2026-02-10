qt_add_executable(client_app
    src/main.cpp
)

qt_add_qml_module(client_app
    URI client
    QML_FILES
        src/ui/Main.qml
)

target_link_libraries(client_app PRIVATE
    shared
    Qt6::Quick
    Qt6::Qml
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
