cmake_minimum_required (VERSION 3.10)


project(random-large-prime)

add_executable(${PROJECT_NAME} 
    main.cpp
)
set(CMAKE_BUILD_TYPE "Release")

find_package(Boost COMPONENTS random)
if(NOT Boost_FOUND)
	set(Boost_INCLUDE_DIRS "K:/1/0/source/boost_1_71_0/build/install/include/boost-1_71")
	set(Boost_LIBRARIES "K:/1/0/source/boost_1_71_0/build/install/lib/libboost_random-mgw81-mt-x32-1_71.a")
endif()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "We are in release")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++17 -O3 -Werror=return-type -Werror=pedantic -pedantic-errors")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s -O3")
	message(STATUS "CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")
else()
	message(STATUS "We are in debug")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17 -Wall -Wextra -Werror=pedantic -pedantic-errors -Werror=return-type -g -O0 -fno-inline -fno-omit-frame-pointer -ggdb" )
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O0")
	message(STATUS "CMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    "${Boost_INCLUDE_DIRS}"
)

target_link_libraries(${PROJECT_NAME}
	"${Boost_LIBRARIES}"
)

