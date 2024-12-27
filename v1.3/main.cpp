#include "./include/AllQtHead.hpp" //所有的Qt头文件

#include "./include/window/fileExplor.hpp"
#include "./include/window/mainWindow.hpp"

 
QString viewDirPath="/home/cl/Desktop/Backup_King/testFiles" ;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

// #include "main.moc" // 如果你使用的是 CMake，可能需要配置 CMakeLists.txt 以启用 MOC
