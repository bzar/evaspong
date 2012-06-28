cmake_minimum_required(VERSION 2.6)
project(pong)

find_package(PkgConfig REQUIRED)

pkg_check_modules(EFL REQUIRED evas ecore eina ecore-evas emotion)
include_directories(${EFL_INCLUDE_DIRS})
add_executable(pong main.cpp)
target_link_libraries(pong ${EFL_LIBRARIES})

