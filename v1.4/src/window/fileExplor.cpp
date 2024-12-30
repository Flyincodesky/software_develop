#include "../../include/window/fileExplor.hpp"

FileExplorer::FileExplorer(const QString &directoryPath , QWidget *parent ) : QWidget(parent) {
    this->curViewPath = directoryPath ;
    this->curSelectedFile = "";
    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建label显示当前路径
    // QLabel *cur_path_label = new QLabel("<span style='font-size: 12pt;'>当前显示路径为：" +this->curViewPath +"</span>" , this); // 调整字体大小版本
    this->cur_path_label = new QLabel("当前显示路径为：" +this->curViewPath , this);
    layout->addWidget(cur_path_label);

    // 新建水平布局 inputLayout ,其文本输入以修改显示的路径
    QHBoxLayout *inputLayout = new QHBoxLayout(); // 水平布局
    QLabel *updata_path_label = new QLabel("修改显示路径为：" , this); //创建文本提示
    inputLayout->addWidget(updata_path_label);
    this->pathInput = new QLineEdit(this);    // 创建文本输入框
    this->pathInput->setPlaceholderText("Enter directory path");
    inputLayout->addWidget(this->pathInput);
    QPushButton *refreshButton = new QPushButton("Enter", this); // 创建确认按钮
    inputLayout->addWidget(refreshButton);
    // 将水平布局 inputLayout ,加入总布局layout
    layout->addLayout(inputLayout , Qt::AlignTop);

    // 创建 up 按钮 ,返回上层目录
    QPushButton *upDirButton = new QPushButton("Up", this); // 创建 up 按钮 ,返回上层目录
    upDirButton->setFixedSize(40, 25); // 设置按钮的大小 
    layout->addWidget(upDirButton , 0, Qt::AlignLeft); // 0表示不使用拉伸，Qt::AlignLeft表示靠左对齐

    // 创建 QFileSystemModel
    this->model = new QFileSystemModel(this);
    this->model->setRootPath(this->curViewPath);
    this->treeView = new QTreeView(this);
    this->treeView->setModel(this->model);
    this->treeView->setRootIndex(this->model->index(this->curViewPath));
    this->treeView->setFixedHeight(200); // 设置 QTreeView 的高度  // 例如设置高度为 200 像素
    layout->addWidget(this->treeView , Qt::AlignTop);

    // 设置列的初始宽度
        this->treeView->setColumnWidth(0, 400); // 设置第一列（Name）的宽度
        this->treeView->setColumnWidth(1, 100); // 设置第二列（Size）的宽度
        this->treeView->setColumnWidth(2, 100); // 设置第三列（Type）的宽度
        this->treeView->setColumnWidth(3, 100); // 设置第四列（Time）的宽度


    // 显示当前选中的文件
    this->cur_selectedFile_label = new QLabel("当前选中的文件为："+this->curSelectedFile , this);
    layout->addWidget( this->cur_selectedFile_label , Qt::AlignTop );
    layout->setAlignment(this->cur_selectedFile_label, Qt::AlignTop);

    setLayout(layout);
    // 调整窗口大小以适应布局
    adjustSize(); // 可选，确保窗口大小自适应布局

    // 连接信号与槽
    connect(refreshButton, &QPushButton::clicked, this, &FileExplorer::refresh);
    connect(upDirButton, &QPushButton::clicked, this, &FileExplorer::upDirPathView);
    connect(treeView, &QTreeView::clicked, this, &FileExplorer::onFileClicked);
    connect(treeView, &QTreeView::doubleClicked, this, &FileExplorer::onFileDoubleClicked); // 连接双击信号
}

// Enter按钮的点击函数, 确认输入的路径
void FileExplorer:: refresh() {
    QString directoryPath = this->pathInput->text();
    if (QFileInfo::exists(directoryPath) && QFileInfo(directoryPath).isDir()) {
        this->curViewPath = directoryPath ;
        this->curSelectedFile = "";
        this->model->setRootPath(this->curViewPath); // 设置显示的路径
        this->treeView->setRootIndex(model->index(this->curViewPath)); // 更新目录树的显示
        this->cur_path_label->setText("当前显示路径为：" +this->curViewPath); // 更新当前路径的显示
        this->cur_selectedFile_label->setText("当前选中的文件为：" +this->curSelectedFile); // 更新目录的时候重置选中的文件
        this->pathInput->clear(); // 清空路径输入框的文本
    }else {
        QMessageBox::warning(this, "Invalid Path", "The specified path does not exist or is not a directory.");
    }
}

// Up按钮的点击函数 , 显示当前目录路径的上级目录
void FileExplorer:: upDirPathView() {
    // 当前路径为 this->curViewPath
    QDir currentDir(this->curViewPath);
    // 获取上级目录
    if (currentDir.cdUp()) { // 检查切换到上级目录是否成功
        this->curViewPath =  currentDir.absolutePath(); // 返回上级目录的绝对路径
        this->curSelectedFile = "";
        this->model->setRootPath(this->curViewPath); // 设置显示的路径
        this->treeView->setRootIndex(model->index(this->curViewPath)); // 更新目录树的显示
        this->cur_path_label->setText("当前显示路径为：" +this->curViewPath); // 更新当前路径的显示
        this->cur_selectedFile_label->setText("当前选中的文件为："+this->curSelectedFile ); // 更新目录的时候重置选中的文件
    } else {
        QMessageBox::warning(this, "Invalid Path", "The current path is the root path, with no upper level path.\n当前路径为根路径，无上级路径。");
    }
}


void FileExplorer:: onFileClicked(const QModelIndex &index) {
    QString filePath = model->filePath(index);
    this->curSelectedFile = filePath ;
    // QMessageBox::information(this, "File Selected", QString("You selected: %1").arg(filePath));
    this->cur_selectedFile_label->setText("当前选中的文件为："+ curSelectedFile ); // 更新目录的时候重置选中的文件
}


void FileExplorer:: onFileDoubleClicked(const QModelIndex &index) {
    // 获取文件路径
    QString filePath = model->filePath(index);
    // 创建 QFileInfo 对象
    QFileInfo fileInfo(filePath);

    // 判断是否为目录
    if (fileInfo.isDir()) {
        // 这里添加打开目录的逻辑，例如更新视图以显示该目录内容
        this->curViewPath = filePath ;
        this->curSelectedFile = "";
        this->model->setRootPath(this->curViewPath); // 设置显示的路径
        this->treeView->setRootIndex(model->index(this->curViewPath)); // 更新目录树的显示
        this->cur_path_label->setText("当前显示路径为：" +this->curViewPath); // 更新当前路径的显示
        this->cur_selectedFile_label->setText("当前选中的文件为："+curSelectedFile ); // 更新目录的时候重置选中的文件
        
    } 
}
