cmake_minimum_required(VERSION 3.27)
project(calculator_project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
add_executable(calculator main.cpp
        calc_app.h
        calc_app.cpp
        event_handlers.cpp
        event_handlers.h
        calculator_core.h
        calculator_core.cpp
        styles.css)
target_include_directories(calculator PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(calculator PRIVATE ${GTK3_LIBRARIES})
target_compile_options(calculator PRIVATE ${GTK3_CFLAGS_OTHER})