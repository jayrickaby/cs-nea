cmake_minimum_required(VERSION 3.15)

project(CdRipper)

set(CMAKE_CXX_STANDARD 20)

add_executable(CdRipper)

target_sources(CdRipper
    PRIVATE
        src/main.cc
)
