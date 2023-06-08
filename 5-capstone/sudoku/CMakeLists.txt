add_library(
    SudokuLib
    "${CMAKE_CURRENT_SOURCE_DIR}/sudoku.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sudoku.h"
    
)

target_include_directories(SudokuLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")