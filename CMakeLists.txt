cmake_minimum_required(VERSION 3.15)
project(TaskScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_scheduler main.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(task_scheduler PRIVATE -Wall -Wextra -pthread)
    target_link_libraries(task_scheduler PRIVATE pthread)
endif()
