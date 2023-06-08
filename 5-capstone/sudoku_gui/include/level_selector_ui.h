#pragma once

#include <vector>
#include <filesystem>

#include <QDialog>
#include <QPushButton>
#include <QWidget>

#include "sudoku.h"

namespace Ui {
class LevelSelectorUi;
}

class LevelSelectorUi : public QDialog
{
    Q_OBJECT
    
public:
    explicit LevelSelectorUi(QWidget* parent, Sudoku* sudoku, bool is_save_dialog, const std::string& search_path);
    ~LevelSelectorUi();
    
private:
    std::vector<std::vector<QPushButton*>> level_buttons_;
    Ui::LevelSelectorUi* ui;
    std::string current_path_{"./level/"};
    Sudoku* active_board_ = nullptr; 
    bool is_save_dialog_ = true;

    void PushedChangeDirectory();
    void SetUpLevelBtns(std::filesystem::path folder_name);
    void ClearLevelBtns();
    void PushedLevelButton();
    void PushedNew();
};