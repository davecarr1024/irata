set(ASM_GEN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/irata_instructions.cpp)
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

add_library(irata_asm
    ${ASM_GEN_OUTPUT}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instruction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instruction_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/irata/asm/instruction_set.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/irata/asm/instruction.hpp
)
add_dependencies(irata_asm generate_asm_cpp)

target_include_directories(irata_asm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Tests for irata_asm

file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS "test/*.cpp")

add_executable(irata_asm_tests ${TEST_SRCS})

add_dependencies(irata_asm_tests generate_asm_cpp)

target_link_libraries(irata_asm_tests irata_asm gtest gmock gtest_main)

target_include_directories(irata_asm_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME asm_tests COMMAND irata_asm_tests)
