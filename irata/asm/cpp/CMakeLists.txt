set(ASM_GEN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/asm.cpp)

add_custom_command(
  OUTPUT ${ASM_GEN_OUTPUT}
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp.py ${ASM_GEN_OUTPUT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../asm.yaml
          ${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp.py
          ${CMAKE_CURRENT_SOURCE_DIR}/../loader.py
  BYPRODUCTS ${ASM_GEN_OUTPUT}
  COMMENT "Generating asm.cpp from asm.yaml"
)

add_custom_target(generate_asm_cpp DEPENDS ${ASM_GEN_OUTPUT})

add_library(irata_asm
    ${ASM_GEN_OUTPUT}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/irata/asm/asm.hpp
)
add_dependencies(irata_asm generate_asm_cpp)

target_include_directories(irata_asm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
