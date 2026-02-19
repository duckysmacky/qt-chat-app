cmake_minimum_required(VERSION 3.16)

project(chat_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup(REQUIRES 6.8)

option(BUILD_CLIENT "Build the client app" ON)
option(BUILD_SERVER "Build the server server" ON)

if(NOT BUILD_CLIENT AND NOT BUILD_SERVER)
    message(FATAL_ERROR "At least one of BUILD_CLIENT or BUILD_SERVER must be ON")
endif()

add_subdirectory(shared)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER)
    add_subdirectory(server)
endif()
