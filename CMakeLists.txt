cmake_minimum_required(VERSION 3.10)
project(common_utils)
set(PROJECT_NAME "common_utils")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(DPDK)
INCLUDE_DIRECTORIES(${DPDK_INCLUDE_DIRS})

include_directories(.)
include_directories(src)
include_directories(src/binio)
include_directories(src/binio/packet)
include_directories(src/binio/tests)
include_directories(src/cli)
include_directories(src/dpdk)
include_directories(src/intrusive)
include_directories(src/intrusive/tests)
include_directories(src/intrusive_pool)
include_directories(src/intrusive_pool/tests)
include_directories(src/logger)
include_directories(src/pcap_utils)
include_directories(src/stack_ip)
include_directories(src/stack_ip/parsers)
include_directories(src/stack_ip/procotols)
include_directories(src/storage)
include_directories(src/storage/tests)
include_directories(src/utils)

set(PROJECT_SOURCE_LIST
        src/binio/tests/test_binio.cpp
        src/cli/tests/test_cli.cpp
        src/intrusive/tests/test_intrusive.cpp
        src/intrusive_pool/tests/test_intrusive_pool.cpp
        src/logger/Logger.cpp
        src/storage/tests/test_storage.cpp
        src/CliContext.cpp
        src/cli/Config.cpp
        src/main.cpp
        src/replay.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_LIST})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DPDK_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} dl pthread)