cmake_minimum_required(VERSION 3.28)
project(chess)

set(CMAKE_CXX_STANDARD 23)

add_executable(cpp_learn main.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/Program Files/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(C:/Program Files/SFML/include/SFML)
target_link_libraries(cpp_learn sfml-system sfml-window sfml-graphics sfml-audio)