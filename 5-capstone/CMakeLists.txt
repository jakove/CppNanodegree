project(Sudoku)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# OR Using the "cmake_find_package" and "cmake_paths" generators
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# potentionalle the OpenCV search path has to be reconfigured
# find_package(OpenCV REQUIRED PATHS "/home/jakob/repos/build/")
find_package(OpenCV REQUIRED PATHS)

add_subdirectory("imageprocessing")
add_subdirectory("sudoku")
add_subdirectory("sudoku_gui")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/models/frozen_mnist_retrained.pb DESTINATION ${CMAKE_BINARY_DIR}/sudoku_gui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/level DESTINATION ${CMAKE_BINARY_DIR}/sudoku_gui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/test DESTINATION ${CMAKE_BINARY_DIR}/sudoku_gui/images)

set(BUILD_TRAINING OFF)

if(BUILD_TRAINING)
    add_subdirectory("training/cpp")    
endif()

