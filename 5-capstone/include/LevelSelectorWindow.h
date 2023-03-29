#pragma once

#include <vector>
#include <filesystem>

#include <QDialog>
#include <QPushButton>
#include <QWidget>

#include "board.h"

namespace Ui {
class LevelSelectorWindow;
}

class LevelSelectorWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit LevelSelectorWindow(QWidget* parent, Board* board, bool is_save_dialog);
    ~LevelSelectorWindow();
    
    //void PushedLevel();

private:
    std::vector<std::vector<QPushButton*>> level_buttons_;
    Ui::LevelSelectorWindow* ui;
    std::string current_path_{"./level/"};
    Board* active_board_ = nullptr; 
    bool is_save_dialog_ = true;

    void PushedChangeDirectory();
    void SetUpLevelBtns(std::filesystem::path folder_name);
    void ClearLevelBtns();
    void PushedLevelButton();
    void PushedNew();
};