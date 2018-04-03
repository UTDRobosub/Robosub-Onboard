cmake_minimum_required(VERSION 3.2)
project(robosub-server CXX)
find_package(OpenCV REQUIRED)
find_package(robosub REQUIRED)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES cpp/*.cpp)
#file(GLOB HEADERS include/*.h)
#include_directories(include)

add_executable(robosub-server ${SOURCES})
target_link_libraries(robosub-server robosub ${robosub_LIBS} ${OpenCV_LIBS})
target_compile_features(robosub-server PRIVATE cxx_range_for)
