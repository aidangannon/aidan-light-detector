cmake_minimum_required(VERSION 3.15)
project(PIC_XC8_Project C)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(CMAKE_C_COMPILER /opt/microchip/xc8/v3.00/bin/xc8-cc)

set(MCU pic16f877a)
set(CFLAGS "-mcpu=${MCU}")
set(HEXFLAGS "-mcpu=${MCU}")

set(SRC
    src/slave_left_right.c
    src/master_up_down.c
)

set(ASM_DIR ${CMAKE_SOURCE_DIR}/asm)

file(MAKE_DIRECTORY ${ASM_DIR})

foreach(SRCFILE IN LISTS SRC)
    get_filename_component(FILENAME ${SRCFILE} NAME_WE)

    set(ASM_FILE ${ASM_DIR}/${FILENAME}.asm)
    set(HEX_FILE ${ASM_DIR}/${FILENAME}.hex)

    add_custom_command(
            OUTPUT ${ASM_FILE}
            COMMAND ${CMAKE_C_COMPILER} ${CFLAGS} -S ${CMAKE_SOURCE_DIR}/${SRCFILE} -o ${ASM_FILE}
            DEPENDS ${CMAKE_SOURCE_DIR}/${SRCFILE}
            COMMENT "Compiling ${SRCFILE} to ASM"
            VERBATIM
    )

    add_custom_command(
            OUTPUT ${HEX_FILE}
            COMMAND ${CMAKE_C_COMPILER} ${HEXFLAGS} ${CMAKE_SOURCE_DIR}/${SRCFILE} -o ${HEX_FILE}
            DEPENDS ${CMAKE_SOURCE_DIR}/${SRCFILE}
            COMMENT "Compiling ${SRCFILE} to HEX"
            VERBATIM
    )

    list(APPEND ASM_FILES ${ASM_FILE})
    list(APPEND HEX_FILES ${HEX_FILE})
endforeach()

add_custom_target(asm_files ALL DEPENDS ${ASM_FILES})

add_custom_target(all_hex ALL DEPENDS ${HEX_FILES})

add_custom_target(asm_flash
        COMMAND pk2cmd -M -PPIC${MCU} -F${ASM_DIR}/main.hex -Y
        DEPENDS all_hex
        COMMENT "Flashing device"
)

add_custom_target(asm_clean
        COMMAND ${CMAKE_COMMAND} -E remove ${ASM_FILES} ${HEX_FILES}
        COMMENT "Cleaning generated files"
)