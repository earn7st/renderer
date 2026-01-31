cmake_minimum_required(VERSION 3.16)
project(renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    set(SDL3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL/include")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

elseif(APPLE)
    find_package(SDL3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)   

endif()