
cmake_minimum_required(VERSION 3.2)

project(Lya)

set(VERSION "3.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# See: https://stackoverflow.com/questions/6468681/failed-linking-to-boost-library-with-cmake-generated-project-file-for-msvc9
add_definitions(-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS ON)

configure_file(${PROJECT_SOURCE_DIR}/src/Program/Configurations.h.in ${PROJECT_SOURCE_DIR}/src/Program/Configurations.h)
set(Boost_DEBUG on)
find_package(Boost 1.64 COMPONENTS system filesystem regex REQUIRED)

add_subdirectory(third_party/glob)
add_subdirectory(third_party/jsoncpp)
add_subdirectory(third_party/grpc)

include_directories(
    src/Program
    src/Extensions
    src/TestFramework
    src/ProtocolBuffers
    third_party/glob
    third_party/grpc/include
)

set(PROGRAM_DIR ${CMAKE_SOURCE_DIR}/src/Program)
set(EXTENSIONS_DIR ${CMAKE_SOURCE_DIR}/src/Extensions)
set(PROTOCOL_BUFFERS_DIR ${CMAKE_SOURCE_DIR}/src/ProtocolBuffers)
file(GLOB PROTOBUFS_SRC ${PROTOCOL_BUFFERS_DIR}/Build/*.cc)

add_executable(lya ${PROJECT_SOURCE_DIR}/src/Program/Exec.cpp ${PROTOBUFS_SRC})
add_executable(lya-extension-javascript ${EXTENSIONS_DIR}/JavaScript/JavaScript.cpp ${PROTOBUFS_SRC})
add_executable(internal-accept-baseline ${PROJECT_SOURCE_DIR}/tasks/AcceptBaseline.cpp)
add_executable(internal-clean-project ${PROJECT_SOURCE_DIR}/tasks/Clean.cpp)
add_executable(internal-diff ${PROJECT_SOURCE_DIR}/tasks/Diff.cpp)
add_executable(internal-generate-diagnostics ${PROJECT_SOURCE_DIR}/tasks/GenerateDiagnostics.cpp)
add_executable(internal-run-tests ${PROJECT_SOURCE_DIR}/src/TestFramework/Exec.cpp)

set_target_properties(
    lya-extension-javascript
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXTENSIONS_DIR}/JavaScript/bin"
)

target_link_libraries(lya glob jsoncpp curl grpc++ grpc ${Boost_LIBRARIES})
target_link_libraries(lya-extension-javascript grpc++ grpc glob jsoncpp ${Boost_LIBRARIES})
target_link_libraries(internal-accept-baseline glob jsoncpp ${Boost_LIBRARIES})
target_link_libraries(internal-clean-project glob jsoncpp ${Boost_LIBRARIES})
target_link_libraries(internal-diff glob ${Boost_LIBRARIES})
target_link_libraries(internal-generate-diagnostics glob jsoncpp ${Boost_LIBRARIES})
target_link_libraries(internal-run-tests glob jsoncpp ${Boost_LIBRARIES})

file(GLOB PROTOS ${CMAKE_SOURCE_DIR}/src/ProtocolBuffers/*.proto)

add_custom_target(
    generate-protobufs
    COMMAND rm -r ${CMAKE_SOURCE_DIR}/src/ProtocolBuffers/Build
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/src/ProtocolBuffers/Build
    COMMAND ${CMAKE_SOURCE_DIR}/bin/protoc -I${CMAKE_SOURCE_DIR}/src/ProtocolBuffers --plugin=protoc-gen-grpc=${CMAKE_SOURCE_DIR}/bin/grpc_cpp_plugin --grpc_out=Build ${PROTOS}
    COMMAND ${CMAKE_SOURCE_DIR}/bin/protoc -I${CMAKE_SOURCE_DIR}/src/ProtocolBuffers --cpp_out=Build ${PROTOS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ProtocolBuffers
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
    generate-diagnostics ./internal-generate-diagnostics
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    DEPENDS internal-generate-diagnostics
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
