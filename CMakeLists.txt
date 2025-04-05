cmake_minimum_required(VERSION 3.30)
project(calculator_project CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(ICU REQUIRED icu-uc)
add_executable(Calculator
        main.cpp
        calc_app.cpp
        event_handlers.cpp
        calculator_core.cpp
)
target_sources(Calculator PRIVATE
        calc_app.h
        event_handlers.h
        calculator_core.h
)
file(COPY styles/styles.css
          styles/dark_styles.css
          styles/icon.png
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(Calculator PRIVATE ${GTK3_INCLUDE_DIRS}
        ${ICU_INCLUDE_DIRS})
target_link_libraries(Calculator PRIVATE ${GTK3_LIBRARIES}
        ${ICU_LIBRARIES})
target_compile_options(Calculator PRIVATE ${GTK3_CFLAGS_OTHER}
        ${ICU_CFLAGS_OTHER})