cmake_minimum_required(VERSION 3.10)

project(MazeGen)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES CONFIGURE_DEPENDS
	"src/*.[hc]pp"
)
add_executable(maze_gen ${SOURCES})

# dependencies
find_package(Threads REQUIRED)

find_package(OpenGL REQUIRED)

if (WIN32)
    include_directories(${WinSDK})
	target_link_libraries(maze_gen ${WinSDK})
endif (WIN32)

target_include_directories(maze_gen PRIVATE "thirdParty/olcPixelGameEngine")
