cmake_minimum_required(VERSION 3.16)

project(chat_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Qml Quick)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(shared)
add_subdirectory(client)
add_subdirectory(server)