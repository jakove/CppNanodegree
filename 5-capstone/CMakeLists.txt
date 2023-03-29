project(Sudoku)

cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# OR Using the "cmake_find_package" and "cmake_paths" generators
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
message("${OpenCV_INCLUDE_DIRS}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(sources 
    "src/SudokuSolver.ui"  
    "src/LevelSelectorWindow.ui"
    "src/main.cpp"
    "src/board.cpp"
    "src/SudokuSolver.cpp"
    "src/LevelHandler.cpp"
    "src/LevelSelectorWindow.cpp"
    "src/SudokuDetector.cpp"
)

# need to include explicitly header files. otherwise a linker error occures: https://github.com/bincrafters/community/issues/466
set(sources 
    ${sources}     
    "include/SudokuSolver.h" 
    "include/LevelSelectorWindow.h")

add_executable("test_board" ${sources})

target_link_libraries("test_board"
    Qt5::Widgets
    ${OpenCV_LIBS}
)

target_include_directories("test_board" PUBLIC 
                           "include")
