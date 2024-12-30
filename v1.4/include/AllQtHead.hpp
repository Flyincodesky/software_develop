#ifndef ALLQTHEAD_HPP  // AllQtHead.hpp
#define ALLQTHEAD_HPP


#include <QApplication>
#include <QWidget>
#include <QScreen> // 包含 QScreen 头文件
#include <QVBoxLayout> // 垂直布局
#include <QHBoxLayout> // 水平布局
#include <QPushButton> // 按钮
#include <QLineEdit>   // 可输入文本框
#include <QLabel>     
#include <QMessageBox> // 消息提示框
#include <QListWidget>

#include <QTreeView>
#include <QFileSystemModel>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QHeaderView>
#include <QInputDialog>
#include <QRegularExpression>
#include <QFileInfo>


// 默认的备份文件存储路径
extern std::string DefaultBackupPath ;

// 记录不同文件的备份信息，用于区分每次备份的是不是已经备份过，以及区分不同文件的备份
extern std::string DefaultBackupInfo ;

// 每个文件备份文件夹里的记录，用于记录每次备份的信息
extern std::string DefaultBackupRecord ;



#endif