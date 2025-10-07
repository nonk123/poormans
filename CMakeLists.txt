cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(poormans LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(poormans INTERFACE)
target_include_directories(poormans INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(WIN32)
    target_link_options(poormans INTERFACE $<IF:$<BOOL:${MSVC}>,/SUBSYSTEM:CONSOLE,-mconsole>)
    target_link_libraries(poormans INTERFACE user32.lib)
endif()

function(add_toy NAME)
    add_executable(poormans_${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/${NAME}.c)
    target_link_libraries(poormans_${NAME} PRIVATE poormans)
endfunction()

function(add_platformer_toy)
    file(GLOB_RECURSE FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/platformer/*.c)
    add_executable(poormans_platformer ${FILES})
    target_link_libraries(poormans_platformer PRIVATE poormans S_fixed)
    target_include_directories(poormans_platformer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platformer)
endfunction()

option(BUILD_TOYS "Build poormans toy programs?" ON)
if(BUILD_TOYS)
    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)

    FetchContent_Declare(S_fixed
        GIT_REPOSITORY https://github.com/Schwungus/S_fixed.git
        GIT_TAG master)
    FetchContent_MakeAvailable(S_fixed)

    add_toy(test)
    add_toy(wolfen)
    add_platformer_toy()
endif()
