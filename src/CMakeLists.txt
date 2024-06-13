cmake_minimum_required(VERSION 3.18)

project(
  RailScheduler
  VERSION 0.1
  LANGUAGES CXX)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
                      "/home/fedir/Developement/RailSheduler/src/or-tools")
find_package(Eigen3 REQUIRED)
find_package(SCIP REQUIRED)
find_package(ortools REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic -g)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(
  ${PROJECT_NAME}
  main.cpp
  route.cpp
  route.h
  track_graph.cpp
  track_graph.h
  schedule.h
  schedule.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(
  ${PROJECT_NAME} PUBLIC $<IF:$<CXX_COMPILER_ID:MSVC>,cxx_std_20,cxx_std_17>)
target_link_libraries(${PROJECT_NAME} PRIVATE ortools::ortools)
