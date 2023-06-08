#pragma once

#include <map>

#include <QMainWindow>
#include <QPushButton>
#include <QFrame>

#include "sudoku.h"

namespace Ui {
class SudokuSolverUi;
}

class SudokuSolverUi : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SudokuSolverUi(QWidget *parent = 0);

private:
    Ui::SudokuSolverUi *ui;
    Sudoku board_ = Sudoku();
    int focus_row_ = -1, focus_col_ = -1;
    QPushButton* focus_button_ = nullptr;

    QPushButton* btn_delete_ = nullptr;
    QPushButton* btn_solve_ = nullptr;
    QPushButton* btn_revert_ = nullptr;
    QPushButton* btn_restart_ = nullptr;

    std::vector<std::vector<QPushButton*>> field_buttons_;
    std::vector<std::pair<QFrame*, QFrame*>> field_lines_;
    std::map<QPushButton*, int> number_buttons_;

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

    void resizeEvent(QResizeEvent* event);

    void RepositionField(int width, int height, int x_offset = 0, int y_offset = 0);

    void RepositionNumberButtons(int width, int height, int x_offset, int y_offset = 0);

    void ConfigureLevel();
};