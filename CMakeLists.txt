
cmake_minimum_required(VERSION 3.2)

if (APPLE)
   set(CMAKE_MACOSX_RPATH ON)
endif (APPLE)

project(Lya)

set(VERSION "3.0")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# See: https://stackoverflow.com/questions/6468681/failed-linking-to-boost-library-with-cmake-generated-project-file-for-msvc9
add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS ON)

configure_file(${PROJECT_SOURCE_DIR}/src/program/configurations.h.in ${PROJECT_SOURCE_DIR}/src/program/configurations.h)
set(Boost_DEBUG on)
find_package(Boost 1.64 COMPONENTS system filesystem regex REQUIRED)

include_directories(
    src/program
    src/extensions
    src/test_runners
    src/protocol_buffers/output
    third_party/grpc/include
    third_party/grpc/third_party/protobuf/src
    third_party/glob
    ${Boost_INCLUDE_DIR}
)

add_subdirectory(third_party/glob)
add_subdirectory(third_party/jsoncpp EXCLUDE_FROM_ALL)
add_subdirectory(third_party/grpc EXCLUDE_FROM_ALL)

set(PROGRAM_DIR ${CMAKE_SOURCE_DIR}/src/program)
set(SERVICES_DIR ${CMAKE_SOURCE_DIR}/src/services)
set(EXTENSIONS_DIR ${CMAKE_SOURCE_DIR}/src/extensions)
set(PROTOCOL_BUFFERS_DIR ${CMAKE_SOURCE_DIR}/src/protocol_buffers)
set(TEST_RUNNERS_DIR ${CMAKE_SOURCE_DIR}/src/test_runners)

set(JAVASCRIPT_EXTENSION_DIR ${EXTENSIONS_DIR}/javascript)

file(GLOB PROTOBUFS_SRC
    ${PROTOCOL_BUFFERS_DIR}/output/*.cc
    ${PROTOCOL_BUFFERS_DIR}/output/*.h)

file(GLOB EXTENSION_SERVER_SRC
    ${SERVICES_DIR}/extension_server/*.cpp
    ${SERVICES_DIR}/extension_server/*.h)

file(GLOB EXTENSION_JAVASCRIPT_SRC
    ${EXTENSIONS_DIR}/javascript/*.cpp
    ${EXTENSIONS_DIR}/javascript/*.h
    ${EXTENSIONS_DIR}/javascript/javascript/extension.json
    ${EXTENSIONS_DIR}/javascript/typescript/extension.json)

file(GLOB PROGRAM_SRC
    ${PROGRAM_DIR}/*.cpp
    ${PROGRAM_DIR}/*.h
    ${PROGRAM_DIR}/*.in
    ${PROGRAM_DIR}/*.json)

file(GLOB TEST_RUNNERS_SRC
    ${TEST_RUNNERS_DIR}/definitions.h
    ${TEST_RUNNERS_DIR}/definitions.cpp
    ${TEST_RUNNERS_DIR}/project_test_runner.h
    ${TEST_RUNNERS_DIR}/project_test_runner.cpp)

add_executable(lya
    ${PROGRAM_SRC}
    ${PROTOBUFS_SRC}
    ${TEST_RUNNERS_SRC})

add_executable(lya-extension-javascript
    ${JAVASCRIPT_EXTENSION_DIR}/javascript/javascript.cpp
    ${PROGRAM_DIR}/utils.cpp
    ${EXTENSION_SERVER_SRC}
    ${EXTENSION_JAVASCRIPT_SRC}
    ${PROTOBUFS_SRC})

add_executable(lya-extension-typescript
    ${JAVASCRIPT_EXTENSION_DIR}/typescript/typescript.cpp
    ${PROGRAM_DIR}/utils.cpp
    ${EXTENSION_SERVER_SRC}
    ${EXTENSION_JAVASCRIPT_SRC}
    ${PROTOBUFS_SRC})

source_group("Extension\\JavaScript" FILES ${EXTENSION_JAVASCRIPT_SRC})
source_group("Extension\\Server" FILES ${EXTENSION_SERVER_SRC})
source_group("ProtocolBuffers" FILES ${PROTOBUFS_SRC})
source_group("Program" FILES ${PROGRAM_DIR}/utils.cpp)

add_executable(internal-accept-baseline ${PROJECT_SOURCE_DIR}/tasks/accept_baseline.cpp ${Boost_LIBRARIES})
add_executable(internal-clean-project ${PROJECT_SOURCE_DIR}/tasks/clean.cpp)
add_executable(internal-diff ${PROJECT_SOURCE_DIR}/tasks/diff.cpp)
add_executable(internal-generate-diagnostics ${PROJECT_SOURCE_DIR}/tasks/generate_diagnostics.cpp)
add_executable(internal-generate-javascript-diagnostics ${PROJECT_SOURCE_DIR}/tasks/generate_javascript_diagnostics.cpp)
add_executable(internal-run-tests ${PROJECT_SOURCE_DIR}/src/test_runners/program.cpp)

set_target_properties(
    lya-extension-javascript
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXTENSIONS_DIR}/javascript/javascript/bin"
)

set_target_properties(
    lya-extension-typescript
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXTENSIONS_DIR}/javascript/typescript/bin"
)

target_link_libraries(lya glob jsoncpp_lib_static curl grpc++ grpc ${Boost_LIBRARIES})
target_link_libraries(lya-extension-javascript grpc++ grpc glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-accept-baseline glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-clean-project glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-diff glob ${Boost_LIBRARIES})
target_link_libraries(internal-generate-diagnostics glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-generate-javascript-diagnostics glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-run-tests glob jsoncpp_lib_static ${Boost_LIBRARIES})

file(GLOB PROTOS ${CMAKE_SOURCE_DIR}/src/protocol_buffers/*.proto)

add_custom_target(
    generate-protobufs
    COMMAND rm -rf ${CMAKE_SOURCE_DIR}/src/protocol_buffers/output
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/src/protocol_buffers/output
    COMMAND ${CMAKE_SOURCE_DIR}/bin/protoc -I${CMAKE_SOURCE_DIR}/src/protocol_buffers --plugin=protoc-gen-grpc=${CMAKE_SOURCE_DIR}/bin/grpc_cpp_plugin --grpc_out=output ${PROTOS}
    COMMAND ${CMAKE_SOURCE_DIR}/bin/protoc -I${CMAKE_SOURCE_DIR}/src/protocol_buffers --cpp_out=output ${PROTOS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/protocol_buffers
    DEPENDS protoc grpc_cpp_plugin
)

add_custom_target(
    run-tests ./internal-run-tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-run-tests
)

add_custom_target(
    diff ./internal-diff
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-diff
)

add_custom_target(
    accept-baseline ./internal-accept-baseline
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-accept-baseline
)

add_custom_target(
    clean-project ./internal-clean-project
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-clean-project
)

add_custom_target(
    generate-diagnostics ./internal-generate-diagnostics
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-generate-diagnostics
)

add_custom_target(
    generate-javascript-diagnostics ./internal-generate-javascript-diagnostics
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-generate-javascript-diagnostics
)

