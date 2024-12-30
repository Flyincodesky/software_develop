#include "./include/AllQtHead.hpp" //所有的Qt头文件

#include "./include/window/fileExplor.hpp"
#include "./include/window/mainWindow.hpp"

 
QString viewDirPath="/home/cl/Desktop/Backup_King/testFiles" ;

// 默认的备份文件存储路径
std::string DefaultBackupPath = "/home/cl/Desktop/Backup_King/BackupDir" ;   //暂不支持自定义

// 记录不同文件的备份信息，用于区分每次备份的是不是已经备份过，以及区分不同文件的备份
// std::string DefaultBackupInfo = "BackUpInfo";  暂不支持自定义

// 每个文件备份文件夹里的记录，用于记录每次备份的信息
std::string DefaultBackupRecord = "BackUpRecord.hwcq307" ;


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

// #include "main.moc" // 如果你使用的是 CMake，可能需要配置 CMakeLists.txt 以启用 MOC
