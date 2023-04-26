cmake_minimum_required(VERSION 3.13)
project(Gamelist)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header files, executable and source files
# include_directories(include)
add_executable(${PROJECT_NAME} main.cpp)
target_sources(${PROJECT_NAME} PRIVATE 
    main.cpp
    )

# Warning levels
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)