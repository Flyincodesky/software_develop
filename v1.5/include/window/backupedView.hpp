#ifndef BACKUPEDVIEW_HPP  // fileExplor.hpp
#define BACKUPEDVIEW_HPP

#include "../AllQtHead.hpp" //所有的Qt头文件
#include "../filetools.hpp"


class BackUpedViewer : public QWidget {
    Q_OBJECT  // 需要这个宏以支持信号和槽
public:
    QTreeView *treeView ;
    QStandardItemModel *model ;
    QString selectRenameFir ;  
    QString selectFirName ;  
    BackUpedViewer(const QString &filePath, QWidget *parent = nullptr) ;

    void  onTreeViewClicked();

};


class BackUpVersionViewer : public QWidget {
        Q_OBJECT  // 需要这个宏以支持信号和槽
public:
    QTreeView *treeView ;
    QStandardItemModel *model ;
    QString selectRenameFir ;  

    QLineEdit *pathInput;
    BackUpVersionViewer(const QString &filePath ,  const QString &WindowTitle ="" ,  QWidget *parent = nullptr) ;

    void  onTreeViewClicked();
};



#endif

