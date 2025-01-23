cmake_minimum_required(VERSION 3.15)
project(PIC_XC8_Project C)

option(USE_XC8 "Build using XC8 compiler and Makefile" OFF)

if(USE_XC8)
    message(STATUS "building with XC8 - delegating to Makefile")

    add_custom_target(
            clean_xc8
            COMMAND ${CMAKE_MAKE_PROGRAM} clean
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "CleaningO with XC8 Makefile"
    )

    add_custom_target(
            build_xc8 ALL
            COMMAND ${CMAKE_MAKE_PROGRAM}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Building with XC8 Makefile"
            DEPENDS clean_xc8
    )
else()
    message(STATUS "buildng with GCC")

    set(CMAKE_C_STANDARD 99)

    file(GLOB SRC_FILES src/*.c)

    add_executable(${PROJECT_NAME} ${SRC_FILES})

    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

    set_target_properties(${PROJECT_NAME} PROPERTIES
            EXCLUDE_FROM_ALL FALSE
    )
endif()
