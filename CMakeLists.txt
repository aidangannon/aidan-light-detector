cmake_minimum_required(VERSION 3.15)
project(PIC_XC8_Project C)

option(USE_XC8 "Build using XC8 compiler and Makefile" OFF)

if(USE_XC8)
    message(STATUS "building with XC8 - delegating to Makefile")

    add_custom_target(
            clean_xc8
            COMMAND ${CMAKE_MAKE_PROGRAM} clean
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Cleaning XC8 build artifacts"
    )

    add_custom_target(
            build_master ALL
            COMMAND ${CMAKE_MAKE_PROGRAM} PROG_NAME=master PROG_DEFINES="-D__MASTER__ -D__MOVEMENTCONFIG__"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Building master program with XC8"
    )

    add_custom_target(
            build_slave ALL
            COMMAND ${CMAKE_MAKE_PROGRAM} PROG_NAME=slave PROG_DEFINES="-D__SLAVE__"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Building slave program with XC8"
    )
else()
    message(STATUS "building with GCC")

     set(CMAKE_C_STANDARD 99)

    # HAL sources for simulation
    file(GLOB HAL_SOURCES "src/hal/*_sim.c")

    add_executable(master
        src/entry.c
        ${HAL_SOURCES}
    )
    target_compile_definitions(master PRIVATE __MASTER__ __MOVEMENTCONFIG__)
    target_include_directories(master PRIVATE src src/hal)

    add_executable(slave
        src/entry.c
        ${HAL_SOURCES}
    )
    target_compile_definitions(slave PRIVATE __SLAVE__)
    target_include_directories(slave PRIVATE src src/hal)

    set_target_properties(master slave PROPERTIES
        EXCLUDE_FROM_ALL FALSE
    )
endif()
