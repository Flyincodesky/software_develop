#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件

#include "../../include/window/fileExplor.hpp"
#include "../../include/window/restoreFile.hpp"

#include "../../include/backuper/backuper.hpp"
#include <string>


extern QString viewDirPath;

class MainWindow : public QWidget {
 public:
    FileExplorer *fileExplorer ;

    MainWindow(QWidget *parent = nullptr);

    // 备份按钮点击
    void backupButtonClicked() ;
    // 查看备份文件按钮点击
    void findBackupFileButtonClicked();
    // 打包按钮点击
    void packButtonClicked();
    // 解包按钮点击
    void unpackButtonClicked();
    // 压缩按钮点击
    void compressButtonClicked();
    // 解压按钮点击
    void uncompressButtonClicked();
    // 加密按钮点击
    void encryptButtonClicked();
    // 解密按钮点击    
    void decryptButtonClicked();
};



 
  
  


#endif