cmake_minimum_required(VERSION 3.12)

project(Examples)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Example03 "03_Maths")

target_link_libraries(Example03 glad glfw)
