#include "board.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <chrono>
#include <string>

Board::Board() {
    for(auto& row : board_) {
        for(auto& val : row) {
            val = 0;
        }
    }

    for(auto& row : init_vals_) {
        for(auto& val : row) {
            val = 0;
        }
    }
}

int Board::Get(int row, int col) {
    if(CheckIndices(row, col)) {
        return board_[row][col];
    }
    return -1;
}

bool Board::IsInitValue(int row, int col) {
    return init_vals_[row][col] > 0;
}

bool Board::Set(int row, int col, int val) {

    if(!(CheckIndices(row, col) && 
        CheckValue(val) &&
        CheckRow(row, col, val) && 
        CheckCol(row, col, val) &&
        CheckBlock(row, col, val))) {
        return false;
    }
    
    steps_.push_back({row, col, board_[row][col]});
    if (steps_.size() > steps_max_size_) {
        steps_.pop_front();
    }
    board_[row][col] = val;
    return true;
}

std::array<int, 3> Board::Revert() {
    if(steps_.size() > 0) {
        auto& last_step = steps_.back();
        steps_.pop_back();
        board_[last_step[0]][last_step[1]] = last_step[2];
        return last_step;
    }
    return {{-1, -1, -1}};
}

bool Board::CheckValue(int val) {
    return val >= 0 && val <= 9;
}

bool Board::CheckCol(int row, int col, int val) {
    for(int row_number = 0; row_number < board_.size(); row_number++) {
        if(row == row_number) {
            continue;
        }
        if(board_[row_number][col] == val) {
            return false;
        }
    }
    return true;
}

bool Board::CheckRow(int row, int col, int val) {
    for(int col_number = 0; col_number < board_.size(); col_number++) {
        if(col == col_number) {
            continue;
        }
        if(board_[row][col_number] == val) {
            return false;
        }
    }
    return true;
}

bool Board::CheckBlock(int row, int col, int val) {
    int block_length = 3;

    int col_start = col - col % block_length;
    int col_end = col_start + block_length;
    int row_start = row - row % block_length;
    int row_end = row_start + block_length;

    for(int col_idx = col_start; col_idx < col_end; col_idx++) {
        for(int row_idx = row_start; row_idx < row_end; row_idx++) {
            if(col_idx == col && row_idx == row) {
                continue;
            }
            if(board_[row_idx][col_idx] == val) {
                return false;
            }
        }
    }
    return true;
}

bool Board::CheckIndices(int row, int col) {
    return row >= 0 && row <= 8 && col >= 0 && col <= 8;
}

bool Board::Delete(int row, int col) {
    if(!CheckIndices(row, col)) {
        std::cout << "Indeces wrong. You entered: x=" << row << ", y=" << col << std::endl;
        return false;
    }
    steps_.push_back({row, col, board_[row][col]});
    if (steps_.size() > steps_max_size_) {
        steps_.pop_front();
    }
    board_[row][col] = 0;
    return true;
}

void Board::PrintBoard() {
    std::cout << "\n";
    for(auto& row : board_) {
        for(auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

bool Board::Finished() {
    for(auto& row : board_) {
        for(auto& val : row) {
            if(val == 0) {
                return false;
            }
        }
    }
    return true;
}

const std::array<std::array<int, 9>, 9>& Board::GetBoard() {
    return board_;
}

bool Board::Solve() {
    CreateLevel();
    const auto start = std::chrono::high_resolution_clock::now();
    bool found_valid_value = true;
    for(int row_number = 0; row_number < board_.size(); row_number++) {
        for(int col_number = 0; col_number < board_[0].size(); col_number++) {
            if(!IsInitValue(row_number, col_number)) {
                int number = board_[row_number][col_number] + 1;
                for(number; number <= 9; number++){
                    found_valid_value = Set(row_number, col_number, number);
                    if (found_valid_value) {
                        break;
                    }
                }
                if (!found_valid_value) {
                    Delete(row_number, col_number);
                }
            }
            if (!found_valid_value) {
                col_number -= 2; // for-loop increases afterwards by one
                if(col_number <= -2) {
                    col_number = 7;
                    if(--row_number < 0) {
                        return false;
                    }
                }
            }
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>((end - start)).count();
    std::cout << "Calculation time: " << duration << " microseconds" << std::endl;
    steps_.clear();
    return true;
}

bool Board::InitValue(int row, int col, int val) {
    if(Set(row, col, val)) {
        init_vals_[row][col] = val;
        return true;
    }
    return false;
}

void Board::CreateLevel() {
    for(auto& row : board_) {
        row.fill(0);
    }
    board_ = init_vals_;
    steps_.clear();
}

bool Board::SaveState(const std::string& path) {
    std::ofstream outfile(path, std::ofstream::out);
    try {
        if(!outfile.is_open()) {
            std::cout << "Error saving file\n";
            return false;
        }
        std::string line_init{}, line_user{};

        for(int row_number = 0; row_number < board_.size(); row_number++) {
            for(int col_number = 0; col_number < board_[0].size(); col_number++) {
                if (IsInitValue(row_number, col_number)) {
                    line_init += std::to_string(Get(row_number, col_number));
                    line_user += "0";
                }
                else {
                    line_user += std::to_string(Get(row_number, col_number));
                    line_init += "0";
                }
            }
        }
        outfile << line_init << "\n" << line_user;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error saving file\n";
        return false;
    }
}

bool Board::LoadState(const std::string& path) {
    std::ifstream infile(path);
    try {
        if(!infile.is_open()) {
            std::cout << "Could not open file to load" << std::endl;
            return false;
        }
        
        std::string init_line{}, user_line{};
        std::getline(infile, init_line);
        std::getline(infile, user_line);

        std::istringstream init_stream(init_line);
        std::istringstream user_stream(user_line);

        int board_size = board_.size() * board_.size();
        if(board_size != init_line.length() || board_size != user_line.length()) {
            std::cout << "Error parsing file" << std::endl;
            return false;
        }

        for(int row_number = 0; row_number < board_.size(); row_number++) {
            for(int col_number = 0; col_number < board_[0].size(); col_number++) {
                int idx = row_number * board_.size() + col_number;
                int init_val = std::stoi(std::string(1, init_line.at(idx)));
                int user_val = std::stoi(std::string(1, user_line.at(idx)));
                init_vals_[row_number][col_number] = init_val;
                board_[row_number][col_number] = (init_val == 0) ? user_val : init_val;
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error loading file: " << e.what() << std::endl;
        return false;
    }
}



