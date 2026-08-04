cmake_minimum_required(VERSION 3.9)
project(dupes)

set(CMAKE_CXX_STANDARD 11)


set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        /Users/rex/.rex/bin
)


add_executable(dupes
        dupes.c
        dupes.h
        Makefile)
