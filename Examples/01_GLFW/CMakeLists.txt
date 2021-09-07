cmake_minimum_required(VERSION 3.12)

project(Examples)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Example01 "01_GLFW")

target_link_libraries(Example01 glad glfw)
