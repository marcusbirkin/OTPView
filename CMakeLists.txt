cmake_minimum_required(VERSION 3.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(gitversion)
project(OTPView LANGUAGES CXX
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions (-DQT_DISABLE_DEPRECATED_BEFORE=0x060000) # disables all the APIs deprecated before Qt 6.0.0

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# Libs
add_subdirectory(libs)

# Main App
add_subdirectory(app)

# Package builder
include(package)