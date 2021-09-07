cmake_minimum_required(VERSION 3.12)

project(Examples)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Example04 "04_SingleBuffer")

target_link_libraries(Example04 glad glfw)
