
cmake_minimum_required(VERSION 3.2)

project(Lya)

set(VERSION "3.0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# See: https://stackoverflow.com/questions/6468681/failed-linking-to-boost-library-with-cmake-generated-project-file-for-msvc9
add_definitions( -DBOOST_ALL_NO_LIB )
set(Boost_USE_STATIC_LIBS ON)

configure_file(${PROJECT_SOURCE_DIR}/src/Program/Configurations.h.in ${PROJECT_SOURCE_DIR}/src/Program/Configurations.h)
set(Boost_DEBUG on)
find_package(Boost 1.64 COMPONENTS system filesystem regex REQUIRED)

add_subdirectory(third_party/glob)
add_subdirectory(third_party/grpc)
add_subdirectory(third_party/jsoncpp)

include_directories("${PROJECT_SOURCE_DIR}/src/Program")
include_directories("${PROJECT_SOURCE_DIR}/src/TestFramework")
include_directories("${PROJECT_SOURCE_DIR}/third_party/glob")

add_executable(lya ${PROJECT_SOURCE_DIR}/src/Program/Exec.cpp)

add_executable(internal-accept-baseline ${PROJECT_SOURCE_DIR}/tasks/AcceptBaseline.cpp)
add_executable(internal-clean-project ${PROJECT_SOURCE_DIR}/tasks/Clean.cpp)
add_executable(internal-diff ${PROJECT_SOURCE_DIR}/tasks/Diff.cpp)
add_executable(internal-generate-diagnostics ${PROJECT_SOURCE_DIR}/tasks/GenerateDiagnostics.cpp)
add_executable(internal-run-tests ${PROJECT_SOURCE_DIR}/src/TestFramework/Exec.cpp)

target_link_libraries(lya glob jsoncpp curl ${Boost_LIBRARIES})

target_link_libraries(internal-accept-baseline glob ${Boost_LIBRARIES})
target_link_libraries(internal-clean-project glob ${Boost_LIBRARIES})
target_link_libraries(internal-diff glob ${Boost_LIBRARIES})
target_link_libraries(internal-generate-diagnostics glob jsoncpp ${Boost_LIBRARIES})
target_link_libraries(internal-run-tests glob ${Boost_LIBRARIES})

list(APPEND PROTOBUFS_SRC
    LyaSync.proto
)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/Program/ProtocolBuffers/Build)

add_custom_target(
    generate-protobufs ../../../bin/protoc --cpp_out=Build ${PROTOBUFS_SRC}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/Program/ProtocolBuffers
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
