FetchContent_Declare(
    qt_secret
    GIT_REPOSITORY https://github.com/quasarapp/qt-secret.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    GIT_SUBMODULES_RECURSE TRUE
)

set(FETCHCONTENT_UPDATES_DISCONNECTED_QT_SECRET ON)
FetchContent_GetProperties(qt_secret)

if(NOT qt_secret_POPULATED)
    set(_qt_secret_existing_src "${FETCHCONTENT_BASE_DIR}/qt_secret-src")

    if(EXISTS "${_qt_secret_existing_src}/src/Qt-RSA/qrsaencryption.cpp")
        set(qt_secret_SOURCE_DIR "${_qt_secret_existing_src}")
    else()
        if(POLICY CMP0169)
            cmake_policy(PUSH)
            cmake_policy(SET CMP0169 OLD)
        endif()

        FetchContent_Populate(qt_secret)

        if(POLICY CMP0169)
            cmake_policy(POP)
        endif()
    endif()
endif()

set(_qt_secret_src "${qt_secret_SOURCE_DIR}/src")
set(_qt_bigint_src "${_qt_secret_src}/mini-gmp/src")

add_library(QtBigint STATIC
    "${_qt_bigint_src}/bigint.cpp"
    "${_qt_bigint_src}/mini-gmp.c"
)

set_source_files_properties("${_qt_bigint_src}/mini-gmp.c" PROPERTIES LANGUAGE CXX)
target_include_directories(QtBigint PUBLIC
    "${_qt_bigint_src}"
)

add_library(Qt-Secret STATIC
    "${_qt_secret_src}/Qt-AES/qaesencryption.cpp"
    "${_qt_secret_src}/Qt-AES/qaesencryption.h"
    "${_qt_secret_src}/Qt-RSA/qrsaencryption.cpp"
)

set_target_properties(Qt-Secret PROPERTIES
    AUTOMOC ON
)

target_include_directories(Qt-Secret PUBLIC
    "${_qt_secret_src}"
    "${_qt_secret_src}/Qt-AES"
    "${_qt_secret_src}/Qt-AES/aesni"
    "${_qt_secret_src}/Qt-RSA"
)

target_compile_definitions(Qt-Secret PUBLIC
    Qt_SECRET_LIBRARY
)

target_link_libraries(Qt-Secret PUBLIC
    Qt6::Core
    QtBigint
)
