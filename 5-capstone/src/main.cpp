#include "SudokuSolver.h"
#include <QApplication>
#include <chrono>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SudokuSolver w;
    w.show();

    return a.exec();
}