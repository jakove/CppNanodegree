#include "level_handler.h"

#include <iostream>
#include <filesystem>
#include <string_view>
#include <algorithm>

std::vector<std::filesystem::path> LevelHandler::GetLevelsFromDirectory(std::string_view directory_path, std::string_view file_extension) {
    std::vector<std::filesystem::path> level;
    
    if(!std::filesystem::exists(directory_path))
    {
        return level;
    }
    for(const auto& path : std::filesystem::directory_iterator(directory_path)) {
        std::string potential_level{};
        if(path.path().extension().generic_u8string() == file_extension) {
            level.emplace_back(path);
        }
    }
    std::sort(level.begin(), level.end());
    return level;
}