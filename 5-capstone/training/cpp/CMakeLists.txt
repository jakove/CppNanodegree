set(executable "SudokuUI")

# OR Using the "cmake_find_package" and "cmake_paths" generators
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED PATHS "/home/jakob/repos/build/")
include_directories(${opencv_INCLUDE_DIRS} )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS -pthread)

set(sources 
    "prepare_training_data.cpp"
)

set(executable "PrepareTrainingData")

add_executable(${executable} ${sources})

target_link_libraries(${executable} 
    ImageProcessing
    ${OpenCV_LIBS}
)