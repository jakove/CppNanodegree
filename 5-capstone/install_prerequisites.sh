
# install gcc-9 and g++9 for c++17 std::filesystem
apt-get update
apt-get -y install g++-9 gcc-9

# build OpenCV 4.X 
mkdir -p /workspace/opencv
cd /workspace/opencv
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.5.zip
unzip opencv.zip
mkdir -p build && cd build
cmake ../opencv-4.5.5
cmake --build .