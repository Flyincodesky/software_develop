#ifndef RESTOREFILE_HPP
#define RESTOREFILE_HPP
#include "../AllQtHead.hpp" //所有的Qt头文件


#include "../../include/window/backupedView.hpp"

extern  std::string DefaultBackupPath ;
extern  std::string DefaultBackupRecord ;

class RestoreWindow : public QWidget {
    Q_OBJECT
 public:
    BackUpedViewer * backupedViewer;


    RestoreWindow(QWidget *parent = nullptr);
    void viewButtonClicked();
   
};










#endif
