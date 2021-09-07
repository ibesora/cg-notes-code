cmake_minimum_required(VERSION 3.12)

project(Examples)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Example02 "02_Triangle")

target_link_libraries(Example02 glad glfw)
