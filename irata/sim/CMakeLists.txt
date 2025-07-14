# Globbing source files
file(GLOB SIM_SRC CONFIGURE_DEPENDS "src/**/*.cc")
file(GLOB SIM_HEADERS CONFIGURE_DEPENDS "include/**/*.h")
file(GLOB SIM_TESTS CONFIGURE_DEPENDS "test/**/*.cc")

# Main library target
add_library(irata-sim ${SIM_SRC} ${SIM_HEADERS})
target_include_directories(irata-sim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Test executable
add_executable(sim_tests ${SIM_TESTS})
target_link_libraries(sim_tests irata-sim gtest gmock gtest_main)

add_test(NAME SimTests COMMAND sim_tests)
