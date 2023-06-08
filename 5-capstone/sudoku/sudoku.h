#pragma once

#include <array>
#include <deque>
#include <vector>


class Sudoku{

  public:
    Sudoku();

    Sudoku(const Sudoku& other_sudoku);

    Sudoku& operator=(const Sudoku& other_sudoku);

    void CreateLevel();
    int Get(int row, int col);
    bool Set(int row, int col, int val);
    bool Finished();
    void PrintSudoku();

    bool Delete(int row, int col);

    bool InitValue(int row, int col, int val);

    bool IsInitValue(int row, int col);

    std::array<int, 3> Revert();

    const std::array<std::array<int, 9>, 9>& GetBoard();  

    bool SaveState(const std::string& path);

    bool LoadState(const std::string& path);

    bool Solve();

    void Clear();

  private:
    std::array<std::array<int, 9>, 9> board_;
    std::array<std::array<int, 9>, 9> init_vals_;
    std::deque<std::array<int, 3>> steps_;
    bool CheckValue(int val);
    bool CheckRow(int row, int col, int val);
    bool CheckCol(int row, int col, int val);
    bool CheckBlock(int row, int col, int val);
    bool CheckIndices(int row, int col);

    const int steps_max_size_ = 1000;
};

