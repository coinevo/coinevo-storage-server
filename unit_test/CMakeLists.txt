cmake_minimum_required (VERSION 3.1)

add_executable (Test
    main.cpp
    storage.cpp
    pow.cpp
    serialization.cpp
    ../httpserver/serialization.cpp
)

set_property(TARGET Test PROPERTY CXX_STANDARD 11)

# library under test

coinevo_add_subdirectory(../storage storage)
coinevo_add_subdirectory(../pow pow)
coinevo_add_subdirectory(../utils utils)
target_link_libraries(Test PRIVATE storage pow utils)
target_include_directories(Test PRIVATE ../httpserver)

# boost
find_package(Boost
REQUIRED
system
filesystem
chrono
thread
unit_test_framework
log
)

set_property(TARGET Test PROPERTY CXX_STANDARD 14)

target_include_directories(Test PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(Test PRIVATE ${Boost_LIBRARIES})
