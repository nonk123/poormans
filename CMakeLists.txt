cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(poormans VERSION 0.1.0
    LANGUAGES C)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(BUILD_SHARED_LIBS OFF)

add_library(poormans INTERFACE)
target_include_directories(poormans
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(WIN32)
    set(CONSOLE_OPT $<IF:$<BOOL:${MSVC}>,/SUBSYSTEM:CONSOLE,-mconsole>)
    target_link_options(poormans INTERFACE ${CONSOLE_OPT})
    target_link_libraries(poormans INTERFACE user32.lib)
endif()

function(add_toy NAME)
    set(TARGET poormans_${NAME})
    add_executable(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/src/${NAME}.c)
    target_link_libraries(${TARGET} PRIVATE poormans)
    set_target_properties(${TARGET} PROPERTIES C_STANDARD 11)
endfunction()

option(POOR_BUILD_TOYS "Build poormans toy programs?")
if(POOR_BUILD_TOYS)
    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)

    set(OSN_FLOAT_TYPE float)
    FetchContent_Declare(S_fixed
        GIT_REPOSITORY https://github.com/Schwungus/S_fixed.git
        GIT_TAG master)
    FetchContent_Declare(noise
        GIT_REPOSITORY https://github.com/nonk123/open-simplex-noise-in-c.git
        GIT_TAG c3862273b9cd7a44d1b2b50c9fc23a9a8cc13992)
    FetchContent_MakeAvailable(S_fixed noise)

    add_toy(test)
endif()
