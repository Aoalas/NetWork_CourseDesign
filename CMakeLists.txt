cmake_minimum_required(VERSION 3.20)

project(NetWork_CourseDesign C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PROJECT_SOURCES
    src/main.c
    src/network_io.c
    src/protocol_parser.c
    src/datalink_sim.c
    src/ftp_server.c
    src/ftp_client.c
    src/utils.c
)

add_executable(network_course_design ${PROJECT_SOURCES})

target_link_libraries(network_course_design ws2_32 iphlpapi)
