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

# In root CMakeLists.txt
add_library(irata_build_flags INTERFACE)

if(IRATA_ENABLE_SANITIZER)
  target_compile_options(irata_build_flags INTERFACE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(irata_build_flags INTERFACE -fsanitize=address -fno-omit-frame-pointer)
endif()

if(IRATA_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(irata_build_flags INTERFACE -fprofile-instr-generate -fcoverage-mapping)
  target_link_options(irata_build_flags INTERFACE -fprofile-instr-generate)
endif()


# ---- Add subprojects ----
add_subdirectory(common)
add_subdirectory(asm)
add_subdirectory(sim)
