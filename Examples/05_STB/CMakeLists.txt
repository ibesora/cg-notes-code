cmake_minimum_required(VERSION 3.12)

project(Examples)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Example05 "05_STB")

target_link_libraries(Example05 glad glfw)
