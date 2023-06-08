#include "sudoku_solver_ui.h"

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/dnn.hpp>

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QAction>

#include "ui_SudokuSolverUi.h"
#include "style.h"
#include "level_selector_ui.h"
#include "imageprocessing.h"

SudokuSolverUi::SudokuSolverUi(QWidget *parent) : QMainWindow(parent),
                                              ui(new Ui::SudokuSolverUi)
{
    ui->setupUi(this);

    board_.CreateLevel();
    const auto &values = board_.GetBoard();

    btn_delete_ = new QPushButton("Delete", this);
    btn_solve_ = new QPushButton("Solve", this);
    btn_revert_ = new QPushButton("Revert", this);
    btn_restart_ = new QPushButton("Restart", this);

    connect(btn_delete_, &QPushButton::released, this, &SudokuSolverUi::PushedDelete);
    connect(btn_solve_, &QPushButton::released, this, &SudokuSolverUi::PushedSolve);
    connect(btn_revert_, &QPushButton::released, this, &SudokuSolverUi::PushedRevert);
    connect(btn_restart_, &QPushButton::released, this, &SudokuSolverUi::PushedRestart);
    connect(ui->actionLoad, &QAction::triggered, this, &SudokuSolverUi::PushedLoad);
    connect(ui->actionSave, &QAction::triggered, this, &SudokuSolverUi::PushedSave);
    connect(ui->actionAI_Sudoku_Loader, &QAction::triggered, this, &SudokuSolverUi::PushedImageProc);

    for (int i = 1; i <= 9; i++)
    {
        auto *button = new QPushButton(QString::number(i), this);
        number_buttons_[button] = i;
        connect(button, &QPushButton::released, this, &SudokuSolverUi::PushedNumber);
    }

    for (int row = 0; row < values.size(); row++)
    {
        std::vector<QPushButton *> button_row;
        for (int col = 0; col < values[0].size(); col++)
        {
            int val = values[row][col];
            auto *button = new QPushButton("", this);
            QString name = "b" + QString::number(row) + QString::number(col);
            button->setObjectName(QString(name));
            button->setStyleSheet(ButtonStyles[ButtonType::kField]);

            if (board_.IsInitValue(row, col))
            {
                button->setText(QString::number(val));
                button->setEnabled(false);
                button->setStyleSheet(ButtonStyles[ButtonType::kInit]);
            }
            connect(button, &QPushButton::released, this, &SudokuSolverUi::PushedField);
            button_row.push_back(button);
        }
        field_buttons_.push_back(button_row);
    }

    for (int i = 0; i <= values.size(); i++)
    {
        auto v_line = new QFrame(this);
        v_line->setObjectName(QString::fromUtf8("line"));
        v_line->setFrameShape(QFrame::HLine);
        v_line->setFrameShadow(QFrame::Plain);
        v_line->setStyleSheet("background-color: black");

        auto h_line = new QFrame(this);
        h_line->setObjectName(QString::fromUtf8("line"));
        h_line->setFrameShape(QFrame::HLine);
        h_line->setFrameShadow(QFrame::Plain);
        h_line->setStyleSheet("background-color: black");

        field_lines_.emplace_back(v_line, h_line);
    }
}

void SudokuSolverUi::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    auto geometry = ui->centralWidget->geometry();

    auto height = geometry.height() - 20;
    auto width = geometry.width() * 2 / 3;
    int size = std::min(height, width);

    double size_d = (size) / 9.0;
    size /= 9;

    RepositionField(size, size, 10, 20);
    RepositionNumberButtons(size, size, size * 9 + 20, 20);
}

void SudokuSolverUi::RepositionNumberButtons(int width, int height, int x_offset, int y_offset)
{
    for (const auto &[button, i] : number_buttons_)
    {
        int row = (i - 1) / 3;
        int col = (i - 1) % 3;

        int x_center = col * (width) + width / 2 + x_offset;
        int y_center = row * (height) + height / 2 + y_offset;

        button->setGeometry(QRect(QPoint(x_center, y_center), QSize(width, height)));
    }

    int x_center = 0 * (width) + width / 2 + x_offset;
    int y_center = 4 * (height) + height / 2 + y_offset;
    btn_revert_->setGeometry(QRect(QPoint(x_center, y_center), QSize(width, height)));
    x_center = 1 * (width) + width / 2 + x_offset;
    btn_delete_->setGeometry(QRect(QPoint(x_center, y_center), QSize(width, height)));
    x_center = 2 * (width) + width / 2 + x_offset;
    btn_restart_->setGeometry(QRect(QPoint(x_center, y_center), QSize(width, height)));
    y_center = 5 * (height) + height / 2 + y_offset;
    x_center = 0 * (width) + width / 2 + x_offset;
    btn_solve_->setGeometry(QRect(QPoint(x_center, y_center), QSize(3 * width, height)));
}

void SudokuSolverUi::RepositionField(int width, int height, int x_offset, int y_offset)
{

    const auto &values = board_.GetBoard();
    for (int row = 0; row < values.size(); row++)
    {
        for (int col = 0; col < values[0].size(); col++)
        {
            int x_center = col * (width) + width / 2 + x_offset;
            int y_center = row * (height) + height / 2 + y_offset;
            auto &button = field_buttons_[row][col];
            button->setGeometry(QRect(QPoint(x_center, y_center), QSize(height, width)));
        }
    }

    for (int i = 0; i < field_lines_.size(); i++)
    {
        auto &[v_line, h_line] = field_lines_[i];
        int x_1 = width / 2 + x_offset;
        int y_1 = i * height + height / 2 + y_offset;
        int line_width = values.size() * height;
        int thicknes = (i % 3 == 0) ? 2 : 1;
        v_line->setGeometry(QRect(x_1, y_1, line_width, thicknes));

        y_1 = width / 2 + y_offset;
        x_1 = i * height + height / 2 + x_offset;
        h_line->setGeometry(QRect(x_1, y_1, thicknes, line_width));
    }
}

void SudokuSolverUi::PushedNumber()
{
    auto *clickedButton = qobject_cast<QPushButton *>(sender());
    auto number = number_buttons_[clickedButton];
    if (!focus_button_)
    {
        return;
    }
    if (board_.Set(focus_row_, focus_col_, number))
    {
        focus_button_->setText(QString::number(number));
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kCorrect]);
    }
    else
    {
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kWrong]);
        return;
    }
    if (board_.Finished())
    {
        QMessageBox msgbox(this);
        msgbox.setText("Congratulations!");
        msgbox.setWindowTitle("Solved");
        msgbox.exec();
    }
}

void SudokuSolverUi::PushedField()
{
    auto *clickedButton = qobject_cast<QPushButton *>(sender());
    SetFocus(clickedButton);
}

void SudokuSolverUi::PushedDelete()
{
    if (focus_button_ == nullptr)
    {
        return;
    }
    if (board_.Delete(focus_row_, focus_col_))
    {
        focus_button_->setText(QString(""));
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kFocus]);
    }
}

void SudokuSolverUi::PushedRevert()
{
    auto last_step = board_.Revert();
    if (last_step[0] > -1)
    {
        auto *button = field_buttons_[last_step[0]][last_step[1]];
        SetFocus(button);
        QString val = (last_step[2] > 0) ? QString::number(last_step[2]) : "";
        focus_button_->setText(val);
    }
}

void SudokuSolverUi::PushedSolve()
{
    if (board_.Solve())
    {
        const auto &board = board_.GetBoard();

        for (int row = 0; row < field_buttons_.size(); row++)
        {
            for (int col = 0; col < field_buttons_[row].size(); col++)
            {
                auto *button = field_buttons_[row][col];
                int number = board[row][col];
                QString text = (number > 0) ? QString::number(number) : "";
                button->setText(text);
            }
        }
    }
    else
    {
        QMessageBox msgbox(this);
        msgbox.setText("No solution could be found.");
        msgbox.setWindowTitle("Error");
        msgbox.exec();
    }
}

void SudokuSolverUi::SetFocus(QPushButton *button)
{
    // reset old focus_button to normal style
    if (focus_button_ != nullptr)
    {
        focus_button_->setStyleSheet(ButtonStyles[ButtonType::kField]);
    }

    focus_button_ = button;
    button->setStyleSheet(ButtonStyles[ButtonType::kFocus]);
    std::string name = button->objectName().toStdString();
    focus_row_ = std::stoi(name.substr(1, 1));
    focus_col_ = std::stoi(name.substr(2, 1));
}

void SudokuSolverUi::PushedRestart()
{
    board_.CreateLevel();
    const auto &board = board_.GetBoard();
    for (int row = 0; row < field_buttons_.size(); row++)
    {
        for (int col = 0; col < field_buttons_[row].size(); col++)
        {
            auto *button = field_buttons_[row][col];
            int number = board[row][col];
            QString text = (number > 0) ? QString::number(number) : "";
            button->setText(text);
        }
    }
}

void SudokuSolverUi::PushedLoad()
{
    LevelSelectorUi *lvl_selector = new LevelSelectorUi(this, &board_, false, "level/");
    lvl_selector->exec();
    FillBoardWithNumbers();
}

void SudokuSolverUi::PushedSave()
{
    LevelSelectorUi *lvl_selector = new LevelSelectorUi(this, &board_, true, "level/");
    lvl_selector->exec();
}

void SudokuSolverUi::FillBoardWithNumbers()
{
    const auto &board = board_.GetBoard();
    for (int row = 0; row < field_buttons_.size(); row++)
    {
        for (int col = 0; col < field_buttons_[row].size(); col++)
        {
            auto *button = field_buttons_[row][col];
            int number = board[row][col];
            if (board_.IsInitValue(row, col))
            {
                button->setEnabled(false);
                button->setStyleSheet(ButtonStyles[ButtonType::kInit]);
            }
            else
            {
                button->setEnabled(true);
                button->setStyleSheet(ButtonStyles[ButtonType::kField]);
            }
            auto text = (number > 0) ? QString::number(number) : "";
            button->setText(text);
        }
    }
}

void SudokuSolverUi::PushedImageProc()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Path to image"), ".");
    if (path.isEmpty())
    {
        return;
    }

    auto orig_image = cv::imread(path.toStdString());
    if (!orig_image.data)
    {
        // Todo global Error handling
        QMessageBox msgbox(this);
        msgbox.setText("There was an error loading the image");
        msgbox.setWindowTitle("Error");
        msgbox.exec();
        return;
    }

    int max_rows = 1024, max_cols = 1024;
    ImageProcessing::DownScaleToMaxSize(orig_image, max_rows, max_cols);

    cv::Mat processed_image;
    cv::cvtColor(orig_image, processed_image, cv::COLOR_BGR2GRAY);

    auto rect_edges = ImageProcessing::FindSudokuField(processed_image);
    if (!rect_edges.has_value())
    {
        // Todo: global Error handling
        QMessageBox msgbox(this);
        msgbox.setText("There was no sudokufield found in the image");
        msgbox.setWindowTitle("Error");
        msgbox.exec();
        return;
    }

    const auto &rect_points = rect_edges.value();
    // from cropping the sodkufield the points are sorted
    ImageProcessing::DrawPolygon(orig_image, rect_points, cv::Scalar(0, 0, 255), 3, true);
    
    // Todo: show image in QT
    cv::imshow("rect", orig_image);
    cv::waitKey(0);

    auto sudoku_image = ImageProcessing::CropSudokuField(processed_image, rect_edges.value());

    auto rois = ImageProcessing::FilterNumberSegments(sudoku_image);
    auto model = cv::dnn::readNetFromTensorflow("frozen_mnist_retrained.pb");
    

    if(rois.size() == 0) {
        // Todo_ global Error handling
        QMessageBox msgbox(this);
        msgbox.setText("No numbers could be found in the image");
        msgbox.setWindowTitle("Error");
        msgbox.exec();
        return;
    }

    Sudoku new_board{};
    bool valid = true;
    for (auto &[position, roi] : rois)
    {
        auto number = ImageProcessing::RecognizeNumber(sudoku_image(roi), model, cv::Size(28, 28), 1 / 255.0);
        valid = valid && new_board.InitValue(position.x, position.y, number);
    }

    // assuming: there has to be an error detecting the numbers correctly if no valid solution could be found 
    valid &= new_board.Solve();
    if(!valid) {
        // Todo: global Error handling
        QMessageBox msgbox(this);
        msgbox.setText("There was an error detecting the numbers correctly");
        msgbox.setWindowTitle("Error");
        msgbox.exec();
        return;
    }

    board_ = new_board;
    board_.CreateLevel();
    FillBoardWithNumbers();
}