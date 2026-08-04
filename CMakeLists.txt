cmake_minimum_required(VERSION 3.9)
project(dupes VERSION 2.0 LANGUAGES C)

add_executable(dupes dupes.c dupes.h)
target_compile_features(dupes PRIVATE c_std_11)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(dupes PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

install(TARGETS dupes RUNTIME DESTINATION bin)
