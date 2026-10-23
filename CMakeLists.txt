cmake_minimum_required(VERSION 3.16)

project(OrganizeX VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(OrganizeX

    src/main.cpp

    src/mainwindow.cpp
    src/mainwindow.h

    src/organizer.cpp

    include/organizer.h
)

target_include_directories(OrganizeX PRIVATE
    include
    src
)

target_link_libraries(OrganizeX PRIVATE
    Qt6::Widgets
)