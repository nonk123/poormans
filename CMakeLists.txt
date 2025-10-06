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

add_toy(test)
add_toy(wolfen)
