cmake_minimum_required(VERSION 3.17)
project(fast_ethernet C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(fast_ethernet
        src/csp.c
        src/csp.h
        src/frame.c
        src/frame.h
        src/station.c
        src/station.h)
