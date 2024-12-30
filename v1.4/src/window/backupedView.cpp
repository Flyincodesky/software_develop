#include "../../include/window/backupedView.hpp"


// 查看有哪些文件备份 
BackUpedViewer::BackUpedViewer(const QString &filePath, QWidget *parent ) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建 QTreeView
    this->treeView = new QTreeView(this);
    layout->addWidget(this->treeView);
    // 创建模型
    this->model = new QStandardItemModel(this);
    // 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open backuped file ");
        std::cout<<filePath.toStdString()<<std::endl;
        return;
    }
    // 读取文件内容
    QTextStream in(&file);

    bool isFirstLine = true;
    // 设置表头（仅一次）
    if (isFirstLine) {
        QStringList list{"首次备份时文件名", "i-node", "首次备份时路径", "备份重命名"};
        this->model->setHorizontalHeaderLabels(list);
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split("////");
        fields.removeAt(2);
        // fields.removeAt(3);
        // 创建每行的项目
        QList<QStandardItem *> items;
        for (const QString &field : fields) {
            items.append(new QStandardItem(field));
        }
        this->model->appendRow(items);
    }
    file.close();
    // 设置模型到 QTreeView
    this->treeView->setModel(this->model);
    
    // 配置 QTreeView
    this->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->treeView->header()->setStretchLastSection(true);
    this->treeView->setAlternatingRowColors(true);  // 可选：设置交替行颜色
    this->treeView->setFixedHeight(350); // 设置 QTreeView 的高度  // 例如设置高度为 200 像素
        // 设置列的初始宽度
        this->treeView->setColumnWidth(0, 150); // 设置第一列（Name）的宽度
        this->treeView->setColumnWidth(1, 80); // 设置第二列（Size）的宽度
        this->treeView->setColumnWidth(2, 320); // 设置第三列（Type）的宽度
        this->treeView->setColumnWidth(3, 100); // 设置第三列（Type）的宽度
    // 设置窗口标题
    // setWindowTitle("List of backed up files");
    // 连接按钮点击信号到槽函数
    connect(treeView, &QTreeView::clicked, this, &BackUpedViewer::onTreeViewClicked );


}

// 连接按钮点击信号到槽函数
void BackUpedViewer:: onTreeViewClicked(){
    QModelIndexList selectedIndexes = treeView->selectionModel()->selectedIndexes();
    if (!selectedIndexes.isEmpty()) {
        // 假设我们只关心第一列的选中项
        // 获取选中项的父项，以确保是同一行的数据
        QModelIndex currentIndex = selectedIndexes.first();
        int row = currentIndex.row();  // 获取行号

        // 遍历列获取该行的所有数据
        for (int column = 0; column < model->columnCount(); ++column) {
            QModelIndex index = model->index(row, column);
            if (index.isValid() && column == 0) {
                this->selectDirName = model->data(index).toString();
            }
            if (index.isValid() && column == 3) {
                this->selectRenameDir = model->data(index).toString();
            }
        }
    }else {
        QMessageBox::warning(this, "No Selection", "No item is selected.");
    }
}








// 查看某个文件的不同备份记录
BackUpVersionViewer::BackUpVersionViewer(const QString &filePath , const QString &WindowTitle , QWidget *parent ) : QWidget(parent) {
    this->filePath = filePath ;
    this->selectDirName   ="";
    this->selectRenameDir =""; // 当前备份的备份文件名
    this->selectBUPath    ="";
    // 获取屏幕信息
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕
    if (!screen){
        QMessageBox::warning(nullptr, "Error", "无法获取屏幕信息"); 
        exit(0);
    }    
    int screenWidth = screen->geometry().width(); // 屏幕宽度
    int screenHeight = screen->geometry().height(); // 屏幕高度
    this->setWindowTitle(WindowTitle+"'s  BackUp Version");
    this->resize(750, 320); // 设置窗口大小
    this->move((screenWidth -  this->width()) / 2, (screenHeight - this->height()) / 2); // 设置窗口初始位置 // 计算窗口位置以居中显示 设置窗口初始位置

    // window 的主要层(垂直)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);



    // 创建 BackUpVersionViewer 实例并添加到布局
    QVBoxLayout *layout = new QVBoxLayout();
    // 创建 QTreeView
    this->treeView = new QTreeView(this);
    layout->addWidget(this->treeView);
    // 创建模型
    this->model = new QStandardItemModel(this);
    // 打开文件
    QString BURecordPath = filePath +"/" + QString::fromStdString(DefaultBackupRecord) ;
    QFile file ( BURecordPath ) ;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Failed to open backuped file ");
        std::cout<<BURecordPath.toStdString()<<std::endl;
        return;
    }
    // 读取文件内容
    QTextStream in(&file);
    QStringList list{"备份时的文件名", "当前备份的备份文件名", "备份时间", "备份时的路径"};
    this->model->setHorizontalHeaderLabels(list);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split("////");
        // fields.removeAt(2);
        // fields.removeAt(3);
        // 创建每行的项目
        QList<QStandardItem *> items;
        for (const QString &field : fields) {
            items.append(new QStandardItem(field));
        }
        this->model->appendRow(items);
    }
    file.close();
    // 设置模型到 QTreeView
    this->treeView->setModel(this->model);
    
    // 配置 QTreeView
    this->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    this->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->treeView->header()->setStretchLastSection(true);
    this->treeView->setAlternatingRowColors(true);  // 可选：设置交替行颜色
    this->treeView->setFixedHeight(200); // 设置 QTreeView 的高度  // 例如设置高度为 200 像素
        // 设置列的初始宽度
        this->treeView->setColumnWidth(0, 140); // 设置第一列（Name）的宽度
        this->treeView->setColumnWidth(1, 140); // 设置第二列（Size）的宽度
        this->treeView->setColumnWidth(2, 140); // 设置第三列（Type）的宽度
        this->treeView->setColumnWidth(3, 200); // 设置第三列（Type）的宽度

    // 添加到主布局
    mainLayout->addLayout(layout);

    // 创建指定路径还原布局
    QHBoxLayout *specialRestoreLayout = new QHBoxLayout();
    QLabel *restore_path_label = new QLabel("指定还原路径为：" , this); //创建文本提示
    specialRestoreLayout->addWidget(restore_path_label);
    this->pathInput = new QLineEdit(this);    // 创建文本输入框
    this->pathInput->setPlaceholderText("Enter directory path");
    specialRestoreLayout->addWidget(this->pathInput);
    QPushButton *specialRestoreButton = new QPushButton("还原到指定路径", this); // 创建确认按钮
    specialRestoreButton->setFixedSize(120,30);
    specialRestoreLayout->addWidget(specialRestoreButton);
    //将水平层加入主要层mainLayout
    mainLayout->addLayout(specialRestoreLayout );


    // 创建查看按钮和取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *restoreButton = new QPushButton("还原到原路径", this);
    restoreButton->setFixedSize(120,30);
    buttonLayout->addWidget(restoreButton );
    QPushButton *cancelButton = new QPushButton("取消", this);
    cancelButton->setFixedSize(120,30);
    buttonLayout->addWidget(cancelButton);
    //将水平层加入主要层mainLayout
    mainLayout->addLayout(buttonLayout );


    setLayout(mainLayout);
    // 连接按钮点击信号到槽函数
    connect(treeView, &QTreeView::clicked, this, &BackUpVersionViewer::onTreeViewClicked );
    connect(restoreButton, &QPushButton::clicked, this, &BackUpVersionViewer:: restoreButtonClicked);
    connect(specialRestoreButton, &QPushButton::clicked, this, &BackUpVersionViewer:: specialRestoreButtonClicked);

    // 连接按钮点击信号到窗口的 close 槽
    connect(cancelButton, &QPushButton::clicked, this, &QWidget::close);
}


// 连接按钮点击信号到槽函数
void BackUpVersionViewer:: onTreeViewClicked(){
    // 要记录第一项备份时文件名，第二项当前备份文件名，第四项备份时路径
    QModelIndexList selectedIndexes = treeView->selectionModel()->selectedIndexes();
    if (!selectedIndexes.isEmpty()) {
        // 假设我们只关心第一列的选中项
        QStringList rowData;
        // 获取选中项的父项，以确保是同一行的数据
        QModelIndex currentIndex = selectedIndexes.first();
        int row = currentIndex.row();  // 获取行号

        this->selectDirName   = model->data(model->index(row, 0)).toString() ;  // 备份时文件名
        this->selectRenameDir = model->data(model->index(row, 1)).toString() ;  // 当前备份的备份文件名
        this->selectBUPath    = model->data(model->index(row, 3)).toString() ; // 备份时的路径

        // QMessageBox::information(this, "Selected Row Data", "You selected: " + selectDirName+selectRenameDir+selectBUPath);
    } else {
        QMessageBox::warning(this, "No Selection", "No item is selected.");
    }
}

// 检查文件名是否合法
bool isValidFileName(const QString &fileName) {
    // 文件名不能包含"/"等非法字符
    QRegularExpression regex("^[^/]*$");
    return regex.match(fileName).hasMatch();
}

// 检查path是否合法: 1上级目录是否存在，不存在就新建 2文件名是否已经存在，存在即重新输入
QString checkPath(const QString &path){
    QString returnPath ;
    //首先检查 selectBUPath 是否合法
        // 上级目录是否存在
    QString parentDir = QFileInfo(path).absolutePath(); // 获取上级目录路径
    QDir dir(parentDir);
    if (!dir.exists(parentDir)) {   // 如果上级目录不存在，尝试创建它
        if (!dir.mkpath(parentDir)) QMessageBox::warning(nullptr, "Error", "Directories not exist and failed to create directories.");
        return "";
    }
        // 文件名是否有重名
    QString fileName = QFileInfo(path).fileName();  // 获取文件名
    // 如果文件名已经存在
    while (dir.exists(fileName)) {
        // 弹出对话框让用户重命名
        bool ok;
        QString newFileName = QInputDialog::getText(nullptr, "File Exists", 
                                                     "The file already exists. Please enter a new name:", 
                                                     QLineEdit::Normal, 
                                                     fileName, &ok);
        if (ok && !newFileName.isEmpty()) {
            // 检查新的文件名是否合法
            if (isValidFileName(newFileName)) {
                fileName = newFileName;  // 更新文件名
                returnPath = dir.absoluteFilePath(fileName);  // 更新完整的路径
                break ;
            } else {
                QMessageBox::warning(nullptr, "Invalid Filename", "The filename contains invalid characters like '/' or other forbidden characters.");
                continue;  // 继续让用户重新输入
            }
        } else if(ok && newFileName.isEmpty() ) continue;
        else return "" ;
        
    }
    // 上级目录存在 且 无同名文件
    return returnPath ;
}

// 还原到备份时的路径
void BackUpVersionViewer:: restoreButtonClicked(){
    if(this->selectBUPath==""||this->selectRenameDir=="" || this->selectDirName ==""){
        QMessageBox::warning(nullptr, "Notice", "未选中备份点，还原失败。"); 
        return;
    }

    // 当前备份文件名 selectRenameDir ，第四项备份时路径 selectBUPath  
    //首先检查 selectBUPath 是否合法
    QString targetfile = checkPath(this->selectBUPath) ;
    if(targetfile=="")return ;
    if(FileTools::copyAllFileKinds( (this->filePath +"/"+ this->selectRenameDir).toStdString() , targetfile.toStdString() ) )QMessageBox::information(nullptr,"Notice", "Restore successfully.\n还原成功."); 
    else QMessageBox::warning(nullptr, "Notice", "Restore failed.\n还原失败."); 

}

// 还原到指定路径
void BackUpVersionViewer:: specialRestoreButtonClicked(){
    if(this->selectBUPath==""||this->selectRenameDir=="" || this->selectDirName ==""){
        QMessageBox::warning(nullptr, "Notice", "未选中备份点，还原失败。"); 
        return ;
    }
    QString directoryPath = this->pathInput->text();
    if (QFileInfo::exists(directoryPath) && QFileInfo(directoryPath).isDir()) {
        //首先检查 selectBUPath 是否合法
        QString targetfile = checkPath(directoryPath) ;
        if(targetfile=="")return ;
            
        if(FileTools::copyAllFileKinds( (this->filePath +"/"+ this->selectRenameDir).toStdString() , targetfile.toStdString() ))QMessageBox::information(nullptr,"Notice", "Restore successfully.\n还原成功."); 
        else QMessageBox::warning(nullptr, "Notice", "Restore failed.\n还原失败."); 
    }else {
        QMessageBox::warning(this, "Invalid Path", "The specified path does not exist or is not a directory.");
        this->pathInput->clear(); // 清空路径输入框的文本
    }
}