cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(irata LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(IRATA_ENABLE_COVERAGE "Enable coverage instrumentation" OFF)

# ---- Fetch GoogleTest ----
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Let CMake download and build it
FetchContent_MakeAvailable(googletest)

enable_testing()

# ---- Add subprojects ----
add_subdirectory(sim)
