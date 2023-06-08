#include "level_selector_ui.h"

#include <iostream>

#include <QFileDialog>
#include <QInputDialog>
#include <QString>

#include "ui_LevelSelectorUi.h"
#include "level_handler.h"


LevelSelectorUi::LevelSelectorUi(QWidget *parent, Sudoku* sudoku, bool save_dialog, const std::string& search_path) : QDialog(parent), active_board_(sudoku), is_save_dialog_(save_dialog), current_path_(search_path), ui(new Ui::LevelSelectorUi) {
    ui->setupUi(this);
    ui->scrollArea->widget()->setLayout(ui->grid);
    
    connect(ui->btn_change_dir, &QPushButton::released, this, &LevelSelectorUi::PushedChangeDirectory);
    
    if(is_save_dialog_) {
        connect(ui->btn_new, &QPushButton::released, this, &LevelSelectorUi::PushedNew); 
    }
    else {
        ui->btn_new->setVisible(false);
    }
    SetUpLevelBtns(current_path_);

}

LevelSelectorUi::~LevelSelectorUi() {

}

void LevelSelectorUi::ClearLevelBtns() {
    while(ui->grid->count() > 0) {
        auto* item = ui->grid->itemAt(0);
        delete item->widget();
    }
}

void LevelSelectorUi::PushedChangeDirectory() {
    QString folder_name = QFileDialog::getExistingDirectory(this, tr("Path to Folder which includes levels"), ".", QFileDialog::ShowDirsOnly);
    if(folder_name.isEmpty()) {
        return;
    }
    SetUpLevelBtns(folder_name.toStdString());
    current_path_ = folder_name.toStdString() + "/";
}

void LevelSelectorUi::PushedNew() {
    bool status = false;
    QString file_name = QInputDialog::getText(this, tr("Levelname"), tr("Insert Levelname to save: "), QLineEdit::Normal, tr("task"), &status);
    if (!(status && !(file_name.isEmpty()))) {
        return;
    }
    // Todo: filename in constants
    active_board_->SaveState(current_path_ + file_name.toStdString() + ".sdku");
    SetUpLevelBtns(current_path_);
}

void LevelSelectorUi::SetUpLevelBtns(std::filesystem::path folder_name) {
    LevelHandler lvl_handler{};
    const auto lvls = lvl_handler.GetLevelsFromDirectory(folder_name.string());
    ClearLevelBtns();
    for(auto& lvl : lvls) {
        auto* button = new QPushButton(QString::fromStdString(lvl.filename().string()), this);
        //Todo:: outsource constants
        button->setFixedHeight(40);
        ui->scrollArea->widget()->layout()->addWidget(button);
        connect(button, &QPushButton::released, this, &LevelSelectorUi::PushedLevelButton);
    }
}

void LevelSelectorUi::PushedLevelButton() {
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