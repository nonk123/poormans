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
    file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/platformer/*.c)
    file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/platformer/*.h)
    add_executable(poormans_platformer ${SOURCES} ${HEADERS})
    target_link_libraries(poormans_platformer PRIVATE poormans S_fixed noise)
    target_include_directories(poormans_platformer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/platformer)
endfunction()

option(BUILD_TOYS "Build poormans toy programs?" ON)
if(BUILD_TOYS)
    set(BUILD_SHARED_LIBS OFF)

    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)

    FetchContent_Declare(S_fixed
        GIT_REPOSITORY https://github.com/Schwungus/S_fixed.git
        GIT_TAG master)
    FetchContent_Declare(noise
        GIT_REPOSITORY https://github.com/smcameron/open-simplex-noise-in-c.git
        GIT_TAG 53cf5bfd4a5289f263568bedd778e2d86eb8a1ef)
    FetchContent_MakeAvailable(S_fixed noise)

    add_library(noise ${noise_SOURCE_DIR}/open-simplex-noise.c)
    target_include_directories(noise PUBLIC ${noise_SOURCE_DIR})
    target_compile_definitions(noise PUBLIC OSNFLOAT=float)

    add_toy(test)
    add_toy(wolfen)
    add_platformer_toy()
endif()
