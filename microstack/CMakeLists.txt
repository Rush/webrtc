cmake_minimum_required(VERSION 3.8)

include(ExternalProject)

ExternalProject_Add(
  openssl
  URL http://www.openssl.org/source/openssl-1.1.0-latest.tar.gz
  CONFIGURE_COMMAND ./config no-shared no-idea no-mdc2 no-rc5 --prefix=${CMAKE_CURRENT_BINARY_DIR}
  BUILD_COMMAND make depend && make
  INSTALL_COMMAND make install_sw
  BUILD_IN_SOURCE 1
)

add_definitions(-D_POSIX)

add_library(openssl_lib STATIC IMPORTED)
set_property(TARGET openssl_lib PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/lib/libssl.a)

add_library(crypto_lib STATIC IMPORTED)
set_property(TARGET crypto_lib PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/lib/libcrypto.a)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# find_package(OpenSSL REQUIRED) 

add_definitions(-D_REMOTELOGGING)

add_library(microstack
  ILibAsyncServerSocket.c  ILibAsyncUDPSocket.c       ILibProcessPipe.c    ILibSimpleDataStore.c  ILibWebRTC.c     ILibWrapperWebRTC.c  sha1.c
  ILibAsyncSocket.c        ILibCrypto.c          ILibParsers.c  ILibRemoteLogging.c  ILibWebClient.c        ILibWebServer.c  md5.c                SHA256.c
)

target_link_libraries(microstack openssl_lib crypto_lib dl util)
add_dependencies(microstack openssl)
# target_add_
