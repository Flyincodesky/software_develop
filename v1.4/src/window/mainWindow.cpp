#include "../../include/window/mainWindow.hpp"



MainWindow::MainWindow(QWidget *parent ) : QWidget(parent) {

    // 获取屏幕信息
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕
    if (!screen){
        QMessageBox::warning(nullptr, "Error", "无法获取屏幕信息"); 
        exit(0);
    }    

    int screenWidth = screen->geometry().width(); // 屏幕宽度
    int screenHeight = screen->geometry().height(); // 屏幕高度
    this->setWindowTitle("Backup");
    this->resize(800, 600); // 设置窗口大小
    this->move((screenWidth -  this->width()) / 2, (screenHeight - this->height()) / 2); // 设置窗口初始位置 // 计算窗口位置以居中显示 设置窗口初始位置

    // window 的主要层(垂直)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建 FileExplorer 实例并添加到布局
    this->fileExplorer = new FileExplorer(viewDirPath, this);
    mainLayout->addWidget(this->fileExplorer);
    mainLayout->setAlignment(this->fileExplorer, Qt::AlignTop);

    // 设置固定高度的 spacer item
    QSpacerItem *spacer1 = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer1);

    // 添加其他控件
    // 创建水平层，增加功能按钮
    QHBoxLayout *backupLayout = new QHBoxLayout();
    QPushButton *backupButton = new QPushButton("备份", this);
    backupButton->setFixedSize(120,30);
    backupLayout->addWidget(backupButton );
    QPushButton *findBackUpFileButton = new QPushButton("查看已备份的文件", this);
    findBackUpFileButton->setFixedSize(120,30);
    backupLayout->addWidget(findBackUpFileButton);
    //将水平层加入主要层mainLayout
    mainLayout->addLayout(backupLayout );

    // 添加固定高度的 spacer item
    QSpacerItem *spacer2 = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(spacer2);

    QVBoxLayout *extendFuncLayout = new QVBoxLayout();
    // 添加伸缩空间，推控件到底部
    extendFuncLayout->addStretch(); 
    QLabel *extendFuncLabel = new QLabel("扩展功能区" , this);  
    extendFuncLayout->addWidget(extendFuncLabel, 0, Qt::AlignHCenter );
    // extendFuncLayout->setAlignment(extendFuncLabel, Qt::AlignBottom);
    // 创建水平层，其他扩展功能按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *packButton = new QPushButton("打包", this);
    buttonLayout->addWidget(packButton);
    buttonLayout->setAlignment(packButton, Qt::AlignTop);
    QPushButton *unpackButton = new QPushButton("解包", this);
    buttonLayout->addWidget(unpackButton);
    buttonLayout->setAlignment(unpackButton, Qt::AlignTop);
    QPushButton *compressButton = new QPushButton("压缩", this);
    buttonLayout->addWidget(compressButton);
    buttonLayout->setAlignment(compressButton, Qt::AlignTop);
    QPushButton *uncompressButton = new QPushButton("解压", this);
    buttonLayout->addWidget(uncompressButton);
    buttonLayout->setAlignment(uncompressButton, Qt::AlignTop);
    QPushButton *encryptButton = new QPushButton("加密", this);
    buttonLayout->addWidget(encryptButton);
    buttonLayout->setAlignment(encryptButton, Qt::AlignTop);
    QPushButton *decryptButton = new QPushButton("解密", this);
    buttonLayout->addWidget(decryptButton);
    buttonLayout->setAlignment(decryptButton, Qt::AlignTop);
    //将垂直层加入主要层mainLayout
    extendFuncLayout->addLayout(buttonLayout );
    
    mainLayout->addLayout(extendFuncLayout );


    setLayout(mainLayout);

    // 连接信号与槽
    connect(backupButton, &QPushButton::clicked, this, &MainWindow::backupButtonClicked);   //备份按钮
    connect(findBackUpFileButton, &QPushButton::clicked, this, &MainWindow::findBackupFileButtonClicked);   //查看备份文件按钮
    connect(packButton, &QPushButton::clicked, this, &MainWindow::packButtonClicked);   //打包按钮
    connect(unpackButton, &QPushButton::clicked, this, &MainWindow::unpackButtonClicked);   //解包按钮
    connect(compressButton, &QPushButton::clicked, this, &MainWindow::compressButtonClicked);   //压缩按钮
    connect(uncompressButton, &QPushButton::clicked, this, &MainWindow::uncompressButtonClicked);   //解压按钮
    connect(encryptButton, &QPushButton::clicked, this, &MainWindow::encryptButtonClicked);   //加密按钮
    connect(decryptButton, &QPushButton::clicked, this, &MainWindow::decryptButtonClicked);   //解密按钮
   

     
}


void MainWindow::backupButtonClicked() {
    // QMessageBox::warning(nullptr, "Error", "备份"); 
    // 检查有没有选中文件
    std::string SelectedFile = this->fileExplorer -> curSelectedFile .toStdString();;
    if( SelectedFile == ""){
        // 未选中文件,报错
        QMessageBox::warning(nullptr, "Notice", "No file selected.\n未选中文件."); 
    }else{
        // 得到了需要备份的文件 string SelectedFile
        if(Backuper::backupFile(SelectedFile))QMessageBox::information(nullptr,"Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup successfully.\n备份成功."); 
        else QMessageBox::warning(nullptr, "Notice", QString::fromStdString(SelectedFile.substr(SelectedFile.find_last_of("/\\") + 1 ) )+" Backup failed.\n备份失败."); 
          
    }
    // 调用函数 ,输入选中文件的路径
}

// 查看备份文件按钮点击
void MainWindow::findBackupFileButtonClicked(){
    // 实例化新窗口，并显示
    RestoreWindow *restoreWindow = new RestoreWindow();
    restoreWindow->show();

}
// 打包按钮点击
void MainWindow::packButtonClicked(){
    QMessageBox::information(nullptr, "Error", "打包"); 

}
// 解包按钮点击
void MainWindow::unpackButtonClicked(){
    QMessageBox::information(nullptr, "Error", "解包"); 

}
// 压缩按钮点击
void MainWindow::compressButtonClicked(){
    QMessageBox::information(nullptr, "Error", "压缩"); 

}
// 解压按钮点击
void MainWindow::uncompressButtonClicked(){
    QMessageBox::information(nullptr, "Error", "解压"); 

}
// 加密按钮点击
void MainWindow::encryptButtonClicked(){
    QMessageBox::information(nullptr, "Error", "加密"); 

}
// 解密按钮点击    
void MainWindow::decryptButtonClicked(){
    QMessageBox::information(nullptr, "Error", "解密"); 


}