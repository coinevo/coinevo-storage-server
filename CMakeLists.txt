cmake_minimum_required(VERSION 3.1)

function(COINEVO_ADD_SUBDIRECTORY SRC)
    get_filename_component(target ${SRC} NAME)
    if (TARGET ${target})
        return()
    endif ()
    add_subdirectory(${SRC} ${ARGN})
endfunction()

project(storage_server)

coinevo_add_subdirectory(utils)
coinevo_add_subdirectory(crypto)
coinevo_add_subdirectory(pow)
coinevo_add_subdirectory(storage)
coinevo_add_subdirectory(httpserver)

if (BUILD_TESTS)
    coinevo_add_subdirectory(unit_test)
endif ()