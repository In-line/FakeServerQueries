cmake_minimum_required(VERSION 3.11)
project(FakeServerQueries)

set(CMAKE_CXX_STANDARD 17)

# src
set(THIRD_PARTY "${CMAKE_SOURCE_DIR}/third_party")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
include_directories(${SRC_DIR})

# api
set(METAMOD "${THIRD_PARTY}/metamod-hl1/metamod")
set(HLSDK "${THIRD_PARTY}/hlsdk")
set(AMXXSDK "${THIRD_PARTY}/amxmodx/public")

include_directories(${AMXXSDK})
include_directories(${AMXXSDK}/sdk)

set(API_INCLUDE_PATH "${THIRD_PARTY} ${HLSDK}/common ${HLSDK}/dlls ${HLSDK}/engine ${HLSDK}/pm_shared ${HLSDK}/game_shared ${HLSDK}/public ${METAMOD}")
string(REPLACE " " ";" API_INCLUDE_PATH_LIST ${API_INCLUDE_PATH})
include_directories(${API_INCLUDE_PATH_LIST})

file(GLOB_RECURSE SOURCE_FILES ${SRC_DIR}/*.cpp ${AMXXSDK}/sdk/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "fsq")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "_amxx_i386.so")

list(REMOVE_ITEM SOURCE_FILES ${SRC_DIR}/SendToHooker.cpp)
add_library(${PROJECT_NAME}_static STATIC ${SOURCE_FILES})

set(FLAGS "-m32 -O3 -pipe -fvisibility-inlines-hidden -fvisibility=hidden -flto ")

set_target_properties(
        ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS
        "${FLAGS} \
        -fno-strict-aliasing \
        -fexceptions \
        -g \
        -Wall \
        -Wextra \
        -Werror \
        -fdata-sections \
        -ffunction-sections \
        -Wunknown-pragmas"
)

set_target_properties(
        ${PROJECT_NAME} PROPERTIES LINK_FLAGS
        "${FLAGS} \
        -Wl,--version-script=${CMAKE_SOURCE_DIR}/version_script.lds \
        -Wl,--gc-sections \
        -static-libstdc++ \
        -static-libgcc"
)

enable_testing()
find_package(GTest REQUIRED)
file(GLOB_RECURSE TEST_SOURCE_FILES test/*.h test/*.cpp)
add_executable(tester ${TEST_SOURCE_FILES})

target_include_directories(tester PUBLIC ${GTEST_INCLUDE_DIR})
target_include_directories(tester PUBLIC ${SRC_DIR} ${SRC_DIR}/helper)
target_link_libraries(tester gtest gtest_main ${PROJECT_NAME}_static)