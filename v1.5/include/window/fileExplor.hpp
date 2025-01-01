#ifndef FILEEXPLOR_HPP  // fileExplor.hpp
#define FILEEXPLOR_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件

extern QString viewDirPath;

class FileExplorer : public QWidget {
    Q_OBJECT
 private:
    
    QLineEdit *pathInput;
    QFileSystemModel *model;
    QTreeView *treeView;
    QLabel *cur_path_label;
    QLabel *cur_selectedFile_label;

 public:
    QString  curViewPath;
    QString  curSelectedFile ;

    FileExplorer(const QString &directoryPath , QWidget *parent = nullptr) ;  //  : QWidget(parent)
    ~FileExplorer(){};
 private slots:
    void refresh(); // Enter按钮的点击函数, 确认输入的路径
    void upDirPathView(); // Up按钮的点击函数 , 显示当前目录路径的上级目录

    void onFileClicked(const QModelIndex &index) ; 
    void onFileDoubleClicked(const QModelIndex &index);

};

#endif

