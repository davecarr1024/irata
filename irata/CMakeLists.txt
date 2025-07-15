cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(irata LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(IRATA_ENABLE_COVERAGE "Enable coverage instrumentation" OFF)
option(IRATA_ENABLE_SANITIZER "Enable ASAN" OFF)


add_subdirectory(extern/googletest)
enable_testing()

# ---- Add subprojects ----
add_subdirectory(asm)
add_subdirectory(sim)
