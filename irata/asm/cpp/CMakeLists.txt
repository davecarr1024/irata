# Generate instructions source from asm.yaml.

set(ASM_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gen)
file(MAKE_DIRECTORY ${ASM_GEN_DIR})
set(ASM_GEN_OUTPUT ${ASM_GEN_DIR}/instruction.cpp)
set(ASM_GEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/../asm.yaml)
add_custom_command(
  OUTPUT ${ASM_GEN_OUTPUT}
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp.py ${ASM_GEN_OUTPUT}
  DEPENDS ${ASM_GEN_INPUT}
          ${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp.py
          ${CMAKE_CURRENT_SOURCE_DIR}/../loader.py
  BYPRODUCTS ${ASM_GEN_OUTPUT}
  COMMENT "Generating ${ASM_GEN_OUTPUT} from ${ASM_GEN_INPUT}"
)
add_custom_target(generate_asm_cpp DEPENDS ${ASM_GEN_OUTPUT})

# Build asm lib.

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS "include/*.hpp")
add_library(irata_asm ${ASM_GEN_OUTPUT} ${SRCS} ${HDRS})
add_dependencies(irata_asm generate_asm_cpp)
target_include_directories(irata_asm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(irata_asm PUBLIC irata_common)
target_link_libraries(irata_asm PRIVATE irata_build_flags)

# Build tests.

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS "test/*.cpp")
add_executable(asm_tests ${TEST_SRCS})
add_dependencies(asm_tests generate_asm_cpp)
target_link_libraries(asm_tests PUBLIC irata_asm irata_common gtest gmock gtest_main)
target_link_libraries(asm_tests PRIVATE irata_build_flags)
add_test(NAME asm_tests COMMAND asm_tests)
