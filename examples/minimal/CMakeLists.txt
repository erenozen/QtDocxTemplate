cmake_minimum_required(VERSION 3.16)
project(qdt_minimal)
add_executable(qdt_minimal main.cpp)
find_package(QtDocxTemplate CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui)

add_executable(minimal main.cpp)
 target_link_libraries(minimal PRIVATE QtDocxTemplate::core Qt6::Core Qt6::Gui)
