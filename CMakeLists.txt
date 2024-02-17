cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME NativeWindow)

project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE TARGET_SOURCE_FILES Source/*.cpp Source/*.mm)

add_library(${TARGET_NAME} STATIC ${TARGET_SOURCE_FILES})

# for test
option(${TARGET_NAME}_BUILD_TEST "Built ${TARGET_NAME} Test" ON)

if(${TARGET_NAME}_BUILD_TEST)
    message("Built ${TARGET_NAME} Test")

    add_executable(${TARGET_NAME}-Test Tests/test.cpp)

    target_include_directories(${TARGET_NAME}-Test PUBLIC ${PROJECT_SOURCE_DIR}/Source)
    target_link_libraries(${TARGET_NAME}-Test ${TARGET_NAME})
    
    if(APPLE)

        set_target_properties(${TARGET_NAME}-Test PROPERTIES LINK_FLAGS "-framework Cocoa")

    endif()
endif()