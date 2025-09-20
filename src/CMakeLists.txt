cmake_minimum_required(VERSION 3.12)
project(somnd)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} somnd.cc)

include_directories(${PROJECT_NAME} ./inc)

add_library(Daemon somnDaemon.cc)   

target_link_libraries(${PROJECT_NAME} Daemon)
target_link_libraries(${PROJECT_NAME} curl)