cmake_minimum_required(VERSION 3.8)

add_definitions(-D_POSIX)
add_subdirectory(microstack)

include_directories(microstack)

add_executable(sample "original-sample/C Sample/WebRTC_MicroStackSample.c" "original-sample/C Sample/SimpleRendezvousServer.c")
target_link_libraries(sample microstack pthread)

add_executable(active active.cpp)
target_link_libraries(active microstack pthread)

add_executable(passive passive.cpp)
target_link_libraries(passive microstack pthread)
