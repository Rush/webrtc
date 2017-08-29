cmake_minimum_required(VERSION 3.8)

add_definitions(-D_POSIX)
add_subdirectory(microstack)

add_executable(sample "webrtc/C Sample/WebRTC_MicroStackSample.c" "webrtc/C Sample/SimpleRendezvousServer.c")

target_link_libraries(sample microstack pthread )
