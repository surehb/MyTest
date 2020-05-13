cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#if(EXISTS "/etc/redhat-release" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
#    set(LIB_SUFFIX 64)
#endif()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/third_party/protobuf/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/g3log/build/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/g3log/src
        ${CMAKE_CURRENT_LIST_DIR}/Common
        ${CMAKE_CURRENT_LIST_DIR}/AsyncContext/
)

link_directories(${PROTOBUF_LIB_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/cmake/build
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/g3log/build
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/cmake/build/third_party/protobuf/
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/cmake/build/third_party/zlib/
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/cmake/build/third_party/cares/cares/lib/
)

# proto
FILE(GLOB COMMON_PROTOS "${CMAKE_CURRENT_SOURCE_DIR}/Protos/*.proto")
foreach(FIL ${COMMON_PROTOS})
    # Proto file
    GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)
    GET_FILENAME_COMPONENT(FIL_WE ${FIL} NAME_WE)
    GET_FILENAME_COMPONENT(ABS_FIL_PATH "${ABS_FIL}" PATH)

    # Generated sources
    SET(FILE_OUTPUT
            "${CMAKE_CURRENT_SOURCE_DIR}/${FIL_WE}.pb.cc"
            "${CMAKE_CURRENT_SOURCE_DIR}/${FIL_WE}.pb.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/${FIL_WE}.grpc.pb.cc"
            "${CMAKE_CURRENT_SOURCE_DIR}/${FIL_WE}.grpc.pb.h"
    )

    LIST(APPEND tmp_generated ${FILE_OUTPUT})

    add_custom_command(
            OUTPUT ${FILE_OUTPUT}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/cmake/build/third_party/protobuf/protoc
            ARGS
            --grpc_out "${CMAKE_CURRENT_SOURCE_DIR}"
            --cpp_out "${CMAKE_CURRENT_SOURCE_DIR}"
            -I "${ABS_FIL_PATH}"
            --plugin=protoc-gen-grpc="${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/grpc/cmake/build/grpc_cpp_plugin"
            "${ABS_FIL}"
    )
endforeach()

add_custom_target(my_test_PB DEPENDS ${tmp_generated})

# Main
add_executable(MyTestService MyTestService.cc Common/Logger.cc ${tmp_generated})
add_dependencies(MyTestService my_test_PB)
target_link_libraries(MyTestService libg3logger.a libprotobuf.a libgrpc++_unsecure.a libgrpc_unsecure.a libgpr.a 
libaddress_sorting.a libcares.a libz.a)

add_executable(MyTestClient MyTestClient.cc Common/Logger.cc ${tmp_generated})
add_dependencies(MyTestClient my_test_PB)
target_link_libraries(MyTestClient libg3logger.a libprotobuf.a libgrpc++_unsecure.a libgrpc_unsecure.a libgpr.a libaddress_sorting.a libcares.a libz.a)

