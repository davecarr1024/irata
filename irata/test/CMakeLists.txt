file(GLOB_RECURSE TEST_PROGRAMS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.asm")

foreach(TEST_FILE ${TEST_PROGRAMS})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  set(BIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.cartridge")

  add_custom_command(
    OUTPUT ${BIN_FILE}
    COMMAND assembler < ${TEST_FILE} > ${BIN_FILE}
    DEPENDS assembler ${TEST_FILE}
    COMMENT "Assembling ${TEST_FILE} -> ${BIN_FILE}"
  )

  add_custom_target(${TEST_NAME}_asm ALL DEPENDS ${BIN_FILE})

  add_test(NAME run_${TEST_NAME} COMMAND sh -c "$<TARGET_FILE:sim> < ${BIN_FILE}")

  set_property(TEST run_${TEST_NAME} PROPERTY DEPENDS ${TEST_NAME}_asm)
endforeach()
