#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件

 
#include "../../include/window/fileExplor.hpp"

extern QString viewDirPath;

class MainWindow : public QWidget {
 public:
    MainWindow(QWidget *parent = nullptr);
};
 


#endif