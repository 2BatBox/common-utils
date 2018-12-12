set(DPDK_INCLUDE_DIR "/usr/adm/include/dpdk")

#IF(CMAKE_BUILD_TYPE MATCHES "Debug")
#    set(DPDK_LIBRARY_DIR "/usr/adm/lib/dpdk-dbg")
#ELSE()
#    set(DPDK_LIBRARY_DIR "/usr/adm/lib/dpdk")
#ENDIF()

set(DPDK_LIBRARY_DIR "/usr/adm/lib/dpdk")

macro(FindRequiredLib LIB LIB_PATH)
    FIND_LIBRARY(${LIB_PATH} ${LIB} PATHS ${DPDK_LIBRARY_DIR})
    if (${LIB_PATH})
        message(STATUS "Library (${LIB}) found in ${${LIB_PATH}}")
    else()
        message(FATAL_ERROR "Library (${LIB}) not found in ${DPDK_LIBRARY_DIR}")
    endif()

endmacro(FindRequiredLib)


# set DPDK_INCLUDE_DIRS
find_path(DPDK_INCLUDE_DIRS rte_config.h
        PATHS . ${DPDK_INCLUDE_DIR})

if (DPDK_INCLUDE_DIRS)
    message(STATUS "Includes (DPDK) found in ${DPDK_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "Includes (DPDK) not found in ${DPDK_INCLUDE_DIR}")
endif ()

# set DPDK_LIBRARIES
set (DPDK_LIBRARIES "-Wl,--whole-archive ")
foreach(rte_lib  rte_mbuf  rte_eal rte_pmd_e1000 rte_mempool rte_ring rte_ethdev rte_pmd_virtio rte_pmd_ixgbe rte_pmd_fm10k rte_pmd_i40e rte_kni rte_net rte_kvargs rte_hash rte_ip_frag)
    FindRequiredLib(${rte_lib} ${rte_lib}_path)
    set (DPDK_LIBRARIES "${DPDK_LIBRARIES} ${${rte_lib}_path}")
endforeach(rte_lib)
set (DPDK_LIBRARIES "${DPDK_LIBRARIES} -Wl,--no-whole-archive" )
