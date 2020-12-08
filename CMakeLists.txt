cmake_minimum_required(VERSION 3.17)
project(fast_ethernet C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(fast_ethernet
        csp.c
        csp.h
        frame_q.c
        frame_q.h
        main.c
        sp.c
        sp.h)
