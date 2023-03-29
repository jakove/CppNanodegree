#pragma once

#include <map>

#include <QMainWindow>
#include <QPushButton>

#include "board.h"

namespace Ui {
class SudokuSolver;
}

class SudokuSolver : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SudokuSolver(QWidget *parent = 0);
    ~SudokuSolver();
    
    void PushedNumber();

    void PushedField();

    void PushedDelete();

    void PushedRevert();

    void PushedSolve();

    void PushedRestart();

    void PushedLoad();

    void PushedSave();

    void PushedImageProc();

    void FillBoardWithNumbers();

    void SetFocus(QPushButton* button);

private:
    Ui::SudokuSolver *ui;
    Board board_ = Board();
    int focus_row_ = -1, focus_col_ = -1;
    QPushButton* focus_button_ = nullptr;

    std::vector<std::vector<QPushButton*>> field_buttons_;
    std::map<QPushButton*, int> number_buttons_;
};