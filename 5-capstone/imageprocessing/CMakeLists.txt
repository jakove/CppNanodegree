add_library(
    ImageProcessing
    "${CMAKE_CURRENT_SOURCE_DIR}/imageprocessing.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imageprocessing.h"
    
)

target_include_directories(ImageProcessing PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(ImageProcessing 
    ${OpenCV_LIBS}
)