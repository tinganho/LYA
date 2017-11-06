
cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

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

include(cmake/modules/FindLibXML++.cmake)
include(cmake/modules/FindGlib.cmake)

include_directories(
    src/program
    src/extensions
    src/test_runners
    src/protos/output
    src/services
    third_party/grpc/include
    third_party/grpc/third_party/protobuf/src
    third_party/glob
    /usr/local/include/glibmm-2.4
    ${LibXML++_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
)

add_subdirectory(third_party/glob)
add_subdirectory(third_party/jsoncpp EXCLUDE_FROM_ALL)
add_subdirectory(third_party/grpc EXCLUDE_FROM_ALL)

set(PROGRAM_DIR ${CMAKE_SOURCE_DIR}/src/program)
set(SERVICES_DIR ${CMAKE_SOURCE_DIR}/src/services)
set(EXTENSIONS_DIR ${CMAKE_SOURCE_DIR}/src/extensions)
set(PROTOS_DIR ${CMAKE_SOURCE_DIR}/src/protos)
set(TEST_RUNNERS_DIR ${CMAKE_SOURCE_DIR}/src/test_runners)
set(JAVASCRIPT_EXTENSION_DIR ${EXTENSIONS_DIR}/javascript)
set(ROOT_FILES
    .editorconfig
    .gitattributes
    .gitignore
    .gitmodules
    .travis.yml
    appveyor.yml
    docker-compose.yml
    readme.md)

set(DTD
    localization.xml
    markup.xml
    views.xml)

file(GLOB PROTOS_SRC
    ${PROTOS_DIR}/output/*.cc
    ${PROTOS_DIR}/output/*.h)

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
    ${PROTOS_SRC}
    ${TEST_RUNNERS_SRC}
    ${ROOT_FILES})

add_executable(lya-extension-javascript
    ${JAVASCRIPT_EXTENSION_DIR}/javascript/javascript.cpp
    ${PROGRAM_DIR}/utils.cpp
    ${EXTENSION_SERVER_SRC}
    ${EXTENSION_JAVASCRIPT_SRC}
    ${PROTOS_SRC})

add_executable(lya-extension-typescript
    ${JAVASCRIPT_EXTENSION_DIR}/typescript/typescript.cpp
    ${PROGRAM_DIR}/utils.cpp
    ${EXTENSION_SERVER_SRC}
    ${EXTENSION_JAVASCRIPT_SRC}
    ${PROTOS_SRC})

source_group("Extension\\JavaScript" FILES ${EXTENSION_JAVASCRIPT_SRC})
source_group("Extension\\Server" FILES ${EXTENSION_SERVER_SRC})
source_group("ProtocolBuffers" FILES ${PROTOS_SRC})
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
target_link_libraries(lya-extension-javascript grpc++ grpc glob jsoncpp_lib_static ${LibXML++_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(internal-accept-baseline glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-clean-project glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-diff glob ${Boost_LIBRARIES})
target_link_libraries(internal-generate-diagnostics glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-generate-javascript-diagnostics glob jsoncpp_lib_static ${Boost_LIBRARIES})
target_link_libraries(internal-run-tests glob jsoncpp_lib_static ${Boost_LIBRARIES})

file(GLOB PROTOS ${CMAKE_SOURCE_DIR}/src/protos/*.proto)

add_custom_target(
    generate-protos
    COMMAND rm -rf ${CMAKE_SOURCE_DIR}/src/protos/output
    COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/src/protos/output
    COMMAND ${CMAKE_SOURCE_DIR}/bin/protoc -I${CMAKE_SOURCE_DIR}/src/protos --plugin=protoc-gen-grpc=${CMAKE_SOURCE_DIR}/bin/grpc_cpp_plugin --grpc_out=output ${PROTOS}
    COMMAND ${CMAKE_SOURCE_DIR}/bin/protoc -I${CMAKE_SOURCE_DIR}/src/protos --cpp_out=output ${PROTOS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/protos
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

