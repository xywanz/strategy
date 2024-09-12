cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_COMPILER /usr/bin/gcc-11)
set(CMAKE_CXX_COMPILER /usr/bin/g++-11)
set(CMAKE_CXX_STANDARD 20)

project(strategy VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "-Wall -fPIC -Wno-stringop-truncation")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overread")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fPIC -DNDEBUG")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib/")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Werror -Wall -Wno-strict-aliasing")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(xyts-strategy-sdk)

set(STRATEGY_SDK_PATH ${PROJECT_SOURCE_DIR}/xyts-strategy-sdk)
set(STRATEGY_PARAM_MGR_GEN_PATH ${STRATEGY_SDK_PATH}/tools/strategy_param_mgr_generator.py)

link_directories(${STRATEGY_SDK_PATH}/lib)
link_libraries(xyts-strategy-sdk)
include_directories(${PROJECT_SOURCE_DIR}/include)

function(gen_param_header_file STRATEGY_NAME)
    set(JSON_NAME "${STRATEGY_NAME}.json")
    set(TARGET_NAME "${STRATEGY_NAME}_param_manager.h")
    add_custom_command(
        OUTPUT  ${TARGET_NAME}
        COMMAND cp ${PROJECT_SOURCE_DIR}/conf/base/${JSON_NAME} ./ &&
                python3 ${STRATEGY_PARAM_MGR_GEN_PATH} ${JSON_NAME} &&
                rm ${JSON_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction(gen_param_header_file)

add_subdirectory(src)

option(BUILD_TESTS "Build the tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()
