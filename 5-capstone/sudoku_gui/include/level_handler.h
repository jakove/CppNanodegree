#pragma once

#include <string>
#include <filesystem>
#include <string_view>

#include "sudoku.h"

class LevelHandler {

public:

    // Todo: change to string_view
    void LoadFromFile(const std::string& path, Sudoku* board);

    std::vector<std::filesystem::path> GetLevelsFromDirectory(std::string_view directory_path, std::string_view file_extension = ".sdku");

    bool SaveToFile(const std::string& path, Sudoku board);

    bool SaveState();
};