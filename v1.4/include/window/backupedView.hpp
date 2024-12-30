#ifndef BACKUPEDVIEW_HPP  // fileExplor.hpp
#define BACKUPEDVIEW_HPP

#include "../AllQtHead.hpp" //所有的Qt头文件
#include "../filetools.hpp"

class BackUpedViewer : public QWidget {
    Q_OBJECT  // 需要这个宏以支持信号和槽
public:
    QTreeView *treeView ;
    QStandardItemModel *model ;
    QString selectRenameDir ;  
    QString selectDirName ;  
    BackUpedViewer(const QString &filePath, QWidget *parent = nullptr) ;

    void  onTreeViewClicked();

};


class BackUpVersionViewer : public QWidget {
        Q_OBJECT  // 需要这个宏以支持信号和槽
public:
    QTreeView *treeView ;
    QStandardItemModel *model ;
    QLineEdit *pathInput;

    QString filePath ;
    QString selectDirName ;   // 备份时文件名
    QString selectRenameDir ;  // 当前备份的备份文件名
    QString selectBUPath ;    // 备份时的路径

    BackUpVersionViewer(const QString &filePath ,  const QString &WindowTitle ="" ,  QWidget *parent = nullptr) ;

    void  onTreeViewClicked();

    // 还原到备份时的路径
    void restoreButtonClicked();
    // 还原到指定路径
    void specialRestoreButtonClicked();

};



#endif

