# delete old build folder
rm -rf build

# build
mkdir build && cd build 
cmake -D CMAKE_BUILD_TYPE=Release CMAKE_C_COMPILER=gcc-9 -D CMAKE_CXX_COMPILER=g++-9 ..
make

# execute
cd sudoku_gui && ./SudokuUi