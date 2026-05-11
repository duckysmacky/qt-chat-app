cmake_minimum_required(VERSION 3.16)

project(chat_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup(REQUIRES 6.8)

include(CTest)
enable_testing()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

function(add_module_tests module_name module_lib)
    file(GLOB test_sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    foreach(test_source IN LISTS test_sources)
        get_filename_component(test_name "${test_source}" NAME_WE)
        string(REPLACE "-" "_" test_id "${test_name}")
        set(test_target "${module_name}_${test_id}")

        add_executable(${test_target}
            ${test_source}
        )

        target_link_libraries(${test_target} PRIVATE
            GTest::gtest_main
            ${module_lib}
        )

        gtest_discover_tests(${test_target}
            TEST_PREFIX "${module_name}."
        )
    endforeach()
endfunction()

option(BUILD_CLIENT "Build the client app" ON)
option(BUILD_SERVER "Build the server server" ON)

if(NOT BUILD_CLIENT AND NOT BUILD_SERVER)
    message(FATAL_ERROR "At least one of BUILD_CLIENT or BUILD_SERVER must be ON")
endif()

if(WIN32)
    get_target_property(_qt_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    function(enable_qt_windows_deploy target)
        if(NOT WINDEPLOYQT_EXECUTABLE)
            message(WARNING "windeployqt not found; ${target} may fail to run due to missing Qt DLLs")
            return()
        endif()

        set(options)
        set(oneValueArgs QML_DIR)
        set(multiValueArgs)
        cmake_parse_arguments(DEPLOY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        set(_deploy_args
            --no-translations
            --no-system-d3d-compiler
            --no-opengl-sw
            --dir "$<TARGET_FILE_DIR:${target}>"
        )

        if(DEPLOY_QML_DIR)
            list(APPEND _deploy_args --qmldir "${DEPLOY_QML_DIR}")
        endif()

        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" ${_deploy_args} "$<TARGET_FILE:${target}>"
            VERBATIM
            COMMAND_EXPAND_LISTS
        )
    endfunction()
endif()

add_subdirectory(shared)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER)
    add_subdirectory(server)
endif()