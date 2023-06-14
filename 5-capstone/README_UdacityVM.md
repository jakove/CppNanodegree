# Build insructions for the UdacityVM

## 1. Dependencies for Running Locally
The code has been successfully tested on UdacityVM with installing the following additional dependencies:
* gcc/g++ >= 9.4.0
* OpenCV >= 4.5.4 

The easiest way to install the dependencies is: `./install_dependencies.sh`
This will install gcc and g++ via apt and build OpenCV at `workspace/opencv/build`

**IMPORTANT** If OpenCV was build manually the search-path for find_package in [CMakeLists.txt]("CMakeLists.txt") has to be adapted!

## 2. Basic Build Instructions
1. Clone this repository
2. Automatically build and execute: `./build_and_execute.sh`