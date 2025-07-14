cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(irata LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(IRATA_ENABLE_COVERAGE "Enable coverage instrumentation" OFF)
option(IRATA_ENABLE_SANITIZER "Enable ASAN" OFF)

# ---- Fetch GoogleTest ----
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
  )

# Let CMake download and build it
FetchContent_MakeAvailable(googletest)

enable_testing()

# ---- Add subprojects ----
add_subdirectory(sim)
