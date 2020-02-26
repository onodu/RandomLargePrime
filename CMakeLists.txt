cmake_minimum_required (VERSION 3.10)

project(random-large-prime)

add_executable(${PROJECT_NAME}
    main.cpp
)

find_package(Boost COMPONENTS random)
if(NOT Boost_FOUND)
    set(Boost_INCLUDE_DIRS "K:/1/0/source/boost_install/include/boost-1_72")
    set(Boost_LIBRARIES "K:/1/0/source/boost_install/lib/libboost_random-mgw81-mt-x32-1_72.a")
endif()
find_package( Threads REQUIRED)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "We are in release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -std=c++17 -O3 -Wall -Werror=return-type -Werror=pedantic -pedantic-errors
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -s -O3
    )
else()
    message(STATUS "We are in debug")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -std=c++17 -Wall -Wextra -Werror=pedantic -pedantic-errors -Werror=return-type -g -O0 -fno-inline -fno-omit-frame-pointer -ggdb
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -O0
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
    Threads::Threads
)

