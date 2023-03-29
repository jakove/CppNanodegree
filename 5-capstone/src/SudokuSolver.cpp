#include "SudokuSolver.h"

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_SudokuSolver.h"
#include "style.h"
#include "LevelSelectorWindow.h"


SudokuSolver::SudokuSolver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SudokuSolver)
{
    ui->setupUi(this);

    board_.CreateLevel();
    const auto& values = board_.GetBoard();

    connect(ui->btn_delete, &QPushButton::released, this, &SudokuSolver::PushedDelete);
    connect(ui->btn_solve, &QPushButton::released, this, &SudokuSolver::PushedSolve);
    connect(ui->btn_revert, &QPushButton::released, this, &SudokuSolver::PushedRevert);
    connect(ui->btn_restart, &QPushButton::released, this, &SudokuSolver::PushedRestart);
    connect(ui->btn_load, &QPushButton::released, this, &SudokuSolver::PushedLoad);
    connect(ui->btn_save, &QPushButton::released, this, &SudokuSolver::PushedSave);
    connect(ui->btn_imageproc, &QPushButton::released, this, &SudokuSolver::PushedImageProc);

    int width = 50, height = 50;
    for(int i = 1; i <= 9; i++) {
        auto* button = new QPushButton(QString::number(i), this);
        number_buttons_[button] = i;
        int x = (i - 1) / 3;
        int y = (i - 1) % 3;

        ui->grid_numbers->addWidget(button, x, y);
        button->setFixedHeight(height);
        button->setFixedWidth(width);
        connect(button, &QPushButton::released, this, &SudokuSolver::PushedNumber);
    }

    
    for(int row = 0; row < values.size(); row++) {
        std::vector<QPushButton*> button_row;
        for(int col = 0; col < values[0].size(); col++) {
            int val = values[row][col];
            auto* button = new QPushButton("", this);
            QString name = "b" + QString::number(row) + QString::number(col);
            button->setObjectName(QString(name));
            button->setFixedHeight(height);
            button->setFixedWidth(width);
            int y_center = col * (width) + width / 2;
            int x_center = row * (height) + height / 2;

            button->setGeometry(QRect(QPoint(y_center, x_center), QSize(height, width)));
            button->setStyleSheet(ButtonStyles[ButtonType::kField]);
            
            if(board_.IsInitValue(row, col)) {
                button->setText(QString::number(val));
                button->setEnabled(false);
                button->setStyleSheet(ButtonStyles[ButtonType::kInit]);
            }
            connect(button, &QPushButton::released, this, &SudokuSolver::PushedField);
            button_row.push_back(button);
        }
        field_buttons_.push_back(button_row);
    }

    for(int i = 0; i <= values.size(); i++) {
        int x_1 = width / 2;
        int y_1 = i * height + height / 2 ;
        int line_width = values.size() * height;
        int thicknes = (i % 3 == 0) ? 2 : 1;
        auto line = new QFrame(this);

        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(x_1, y_1, line_width, thicknes));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Plain);
        line->setStyleSheet("background-color: black");

        y_1 = x_1;
        x_1 = i * height + height / 2;

        line = new QFrame(this);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(x_1, y_1, thicknes, line_width));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Plain);
        line->setStyleSheet("background-color: black"); 
    }
}

SudokuSolver::~SudokuSolver()
{
    delete ui;
}

void SudokuSolver::PushedNumber() {
    auto* clickedButton = qobject_cast<QPushButton*>(sender());
    auto number = number_buttons_[clickedButton];
    if(!focus_button_) {
        return;
    }
    if(board_.Set(focus_row_, focus_col_, number)) {
        focus_button_->setText(QString::number(number));
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kCorrect]);
    }
    else{
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kWrong]);
        return;
    }
    if (board_.Finished()) {
        std::cout << "solved" << std::endl;
        QMessageBox msgbox(this);
        msgbox.setText("Congratulations!");
        msgbox.setWindowTitle("Solved");
        //msgbox.setParent(this);
        msgbox.exec();
    }
}

void SudokuSolver::PushedField() {
    auto* clickedButton = qobject_cast<QPushButton*>(sender());
    SetFocus(clickedButton);
}

void SudokuSolver::PushedDelete() {
    if(!focus_button_) {
        return;
    }
    if(board_.Delete(focus_row_, focus_col_)) {
        focus_button_->setText(QString(""));
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kFocus]);
    }
}

void SudokuSolver::PushedRevert() {
    auto last_step = board_.Revert();
    if(last_step[0] > -1) { 
        auto* button = field_buttons_[last_step[0]][last_step[1]];
        SetFocus(button);
        QString val = (last_step[2] > 0) ? QString::number(last_step[2]) : "";
        focus_button_->setText(val);
    }
}

void SudokuSolver::PushedSolve() {
    if(board_.Solve()) {
        const auto& board = board_.GetBoard();
        
        for(int row = 0; row < field_buttons_.size(); row++) {
            for(int col = 0; col < field_buttons_[row].size(); col++) {
                auto* button = field_buttons_[row][col];
                int number = board[row][col];
                QString text = (number > 0) ? QString::number(number) : "";
                button->setText(text);
            }   
        }
    }
}

void SudokuSolver::SetFocus(QPushButton* button) {
    if(focus_button_) {
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kField]);
    }

    focus_button_ = button;
    button->setStyleSheet(ButtonStyles[ButtonType::kFocus]);
    std::string name = button->objectName().toStdString();
    focus_row_ = std::stoi(name.substr(1, 1));
    focus_col_ = std::stoi(name.substr(2, 1));
}

void SudokuSolver::PushedRestart() {
    board_.CreateLevel();
    const auto& board = board_.GetBoard();
    for(int row = 0; row < field_buttons_.size(); row++) {
        for(int col = 0; col < field_buttons_[row].size(); col++) {
            auto* button = field_buttons_[row][col];
            int number = board[row][col];
            QString text = (number > 0) ? QString::number(number) : "";
            button->setText(text);
        }
    }
}

void SudokuSolver::PushedLoad() {
    LevelSelectorWindow* lvl_selector = new LevelSelectorWindow(this, &board_, false);
    lvl_selector->exec();
    FillBoardWithNumbers();
}

void SudokuSolver::PushedSave() {
    LevelSelectorWindow* lvl_selector = new LevelSelectorWindow(this, &board_, true);
    lvl_selector->exec();
}

void SudokuSolver::FillBoardWithNumbers() {
    const auto& board = board_.GetBoard();
    for(int row = 0; row < field_buttons_.size(); row++) {
        for(int col = 0; col < field_buttons_[row].size(); col++) {
            auto* button = field_buttons_[row][col];
            int number = board[row][col];
            if(board_.IsInitValue(row, col)) {
                button->setEnabled(false);
                button->setStyleSheet(ButtonStyles[ButtonType::kInit]);
            }
            else {
                button->setEnabled(true);
                button->setStyleSheet(ButtonStyles[ButtonType::kField]);
            }
            QString text = (number > 0) ? QString::number(number) : "";
            button->setText(text);
        }
    }
}

void SudokuSolver::PushedImageProc() {
    QString folder_name = QFileDialog::getExistingDirectory(this, tr("Path to image"), ".");
    if(folder_name.isEmpty()) {
        return;
    }
}