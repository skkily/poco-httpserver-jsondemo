cmake_minimum_required(VERSION 3.0.0)
project(demo VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(demo main.cpp)

target_link_libraries(demo pthread)
target_link_libraries(demo PocoNet)
target_link_libraries(demo PocoFoundation)
target_link_libraries(demo PocoUtil)
target_link_libraries(demo PocoJSON)

