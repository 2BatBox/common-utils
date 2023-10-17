cmake_minimum_required(VERSION 3.10)
project(common_utils)
set(PROJECT_NAME "common-utils")

##################
# common section #
##################
set(DEBUG_LEVEL "-g3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${DEBUG_LEVEL} -Wall")

include_directories(src/lib)

################
# applications #
################

# autotest
set(APP_AUTOTEST_NAME "autotest")
set(APP_AUTOTEST_SOURCE
        src/test/autotest.cpp
        )

add_executable(${APP_AUTOTEST_NAME} ${APP_AUTOTEST_SOURCE})
target_link_libraries(${APP_AUTOTEST_NAME})

# sample-pcap
set(APP_SAMPLE_PCAP_NAME "sample-pcap")
set(APP_SAMPLE_PCAP_SOURCE
        src/samples/pcap.cpp
        )

add_executable(${APP_SAMPLE_PCAP_NAME} ${APP_SAMPLE_PCAP_SOURCE})
target_link_libraries(${APP_SAMPLE_PCAP_NAME} pcap)

# sample-proto
set(APP_SAMPLE_PROTO_NAME "sample-proto")
set(APP_SAMPLE_PROTO_SOURCE
        src/samples/proto-dump.cpp
        )

add_executable(${APP_SAMPLE_PROTO_NAME} ${APP_SAMPLE_PROTO_SOURCE})
target_link_libraries(${APP_SAMPLE_PROTO_NAME} pcap)

# main
set(APP_SAMPLE_MAIN_NAME "main")
set(APP_SAMPLE_MAIN_SOURCE
        src/samples/main.cpp
        )

add_executable(${APP_SAMPLE_MAIN_NAME} ${APP_SAMPLE_MAIN_SOURCE})
target_link_libraries(${APP_SAMPLE_PROTO_NAME})
