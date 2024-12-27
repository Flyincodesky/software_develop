#include "../../include/window/mainWindow.hpp"


MainWindow::MainWindow(QWidget *parent ) : QWidget(parent) {

    // 获取屏幕信息
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕
    if (!screen){
        QMessageBox::information(nullptr, "Error", "无法获取屏幕信息"); 
        exit(0);
    }    

    int screenWidth = screen->geometry().width(); // 屏幕宽度
    int screenHeight = screen->geometry().height(); // 屏幕高度
    this->setWindowTitle("Backup");
    this->resize(800, 600); // 设置窗口大小
    this->move((screenWidth -  this->width()) / 2, (screenHeight - this->height()) / 2); // 设置窗口初始位置 // 计算窗口位置以居中显示 设置窗口初始位置

    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建 FileExplorer 实例并添加到布局
   
    FileExplorer *fileExplorer = new FileExplorer(viewDirPath, this);
    mainLayout->addWidget(fileExplorer);

    // 添加其他控件
    QPushButton *otherButton = new QPushButton("Other Button", this);
    mainLayout->addWidget(otherButton);

    setLayout(mainLayout);
     
}