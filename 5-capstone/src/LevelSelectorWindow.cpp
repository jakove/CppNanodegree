#include "LevelSelectorWindow.h"

#include <iostream>

#include <QFileDialog>
#include <QInputDialog>
#include <QString>

#include "ui_LevelSelectorWindow.h"
#include "LevelHandler.h"


LevelSelectorWindow::LevelSelectorWindow(QWidget *parent, Board* board, bool save_dialog) : QDialog(parent), active_board_(board), is_save_dialog_(save_dialog), ui(new Ui::LevelSelectorWindow) {
    ui->setupUi(this);
    //ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->widget()->setLayout(ui->grid);
    
    connect(ui->btn_change_dir, &QPushButton::released, this, &LevelSelectorWindow::PushedChangeDirectory);
    
    if(is_save_dialog_) {
        connect(ui->btn_new, &QPushButton::released, this, &LevelSelectorWindow::PushedNew); 
    }
    else {
        ui->btn_new->setVisible(false);
    }
    SetUpLevelBtns(current_path_);
    //ui->scrollArea->widget()->layout()->addWidget(ui->grid);

}

LevelSelectorWindow::~LevelSelectorWindow() {

}

void LevelSelectorWindow::ClearLevelBtns() {
    while(ui->grid->count() > 0) {
        auto* item = ui->grid->itemAt(0);
        delete item->widget();
    }
}

void LevelSelectorWindow::PushedChangeDirectory() {
    QString folder_name = QFileDialog::getExistingDirectory(this, tr("Path to Folder which includes levels"), ".", QFileDialog::ShowDirsOnly);
    if(folder_name.isEmpty()) {
        return;
    }
    SetUpLevelBtns(folder_name.toStdString());
    current_path_ = folder_name.toStdString();
}

void LevelSelectorWindow::PushedNew() {
    bool status = false;
    QString file_name = QInputDialog::getText(this, tr("Levelname"), tr("Insert Levelname to save: "), QLineEdit::Normal, tr("task"), &status);
    if (!(status && !(file_name.isEmpty()))) {
        return;
    }
    // Todo: filename in constants
    active_board_->SaveState(current_path_ + file_name.toStdString() + ".sdku");
    SetUpLevelBtns(current_path_);
}

void LevelSelectorWindow::SetUpLevelBtns(std::filesystem::path folder_name) {
    LevelHandler lvl_handler{};
    const auto lvls = lvl_handler.GetLevelsFromDirectory(folder_name.string());
    ClearLevelBtns();
    for(auto& lvl : lvls) {
        auto* button = new QPushButton(QString::fromStdString(lvl.filename().string()), this);
        //Todo:: outsource constants
        button->setFixedHeight(40);
        ui->scrollArea->widget()->layout()->addWidget(button);
        connect(button, &QPushButton::released, this, &LevelSelectorWindow::PushedLevelButton);
    }
}

void LevelSelectorWindow::PushedLevelButton() {
    auto* levelButton = qobject_cast<QPushButton*>(sender());
    auto level = levelButton->text().toStdString();
    if (is_save_dialog_){
        active_board_->SaveState(current_path_ + level);
    }
    else {
        active_board_->LoadState(current_path_ + level);
    }
    this->close();
}