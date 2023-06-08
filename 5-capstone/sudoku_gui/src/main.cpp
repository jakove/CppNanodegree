#include <QApplication>
#include <chrono>

#include "sudoku_solver_ui.h"


int main(int argc, char *argv[])
{
    //ImageProcessing::DetectSudokuInImage("/home/jakob/Documents/git/CppNanodegree/5-capstone/resources/images/training/Screenshot 2023-04-29 at 21-15-54 3-11-07 C'mon Bob...lets play!.png", true);
    
    QApplication a(argc, argv);
    SudokuSolverUi w;
    w.show();
    return a.exec();
}