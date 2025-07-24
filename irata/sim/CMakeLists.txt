# Globbing source files
file(GLOB_RECURSE SIM_SRC CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE SIM_HEADERS CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE SIM_TESTS CONFIGURE_DEPENDS "test/*.cpp")

# Main library target
add_library(irata_sim ${SIM_SRC} ${SIM_HEADERS})
target_include_directories(irata_sim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(irata_sim PUBLIC irata_asm)

# Test executable
add_executable(sim_tests ${SIM_TESTS})
target_link_libraries(sim_tests irata_sim irata_asm gtest gmock gtest_main)

add_test(NAME sim_tests COMMAND sim_tests)

# Apply coverage flags only to irata targets
if(IRATA_ENABLE_COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(irata_sim PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(irata_sim PRIVATE -fprofile-instr-generate)

    target_compile_options(sim_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(sim_tests PRIVATE -fprofile-instr-generate)
  endif()
endif()

if(IRATA_ENABLE_SANITIZER)
  target_compile_options(irata_sim PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(irata_sim PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_compile_options(sim_tests PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(sim_tests PRIVATE -fsanitize=address -fno-omit-frame-pointer)
endif()
