cmake_minimum_required(VERSION 3.10)

project(random-large-prime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cpp)
find_package(Boost REQUIRED COMPONENTS random thread chrono)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::random Boost::thread Boost::chrono)
