#ifndef BACKUPER_HPP
#define BACKUPER_HPP

#include "../filetools.hpp"

class Backuper : public FileTools{
 public:
    Backuper(){};
    ~Backuper(){};

    //备份
	bool backupRegFile(std::string source, std::string destination); // 备份普通文件，便于moveFile以及backupDir调用
    bool backupSymLINK(std::string sourcefile, std::string targetfile); // 备份软链接(符号链接)
    bool backupHardLINK(std::string  sourceFile, std::string targetFile); // 备份硬链接文件 
    bool backupFIFO(std::string sourcefile, std::string targetfile); // 备份管道
    bool backupDir(std::string sourceDir, std::string targetDir);  // 包含四种文件类型的目录备份功能
    bool backupAllFile(std::string sourceFile, std::string targetFile);    // 用于整合备份四种文件，管道，链接，普通，目录
    
    // 兼容性移动文件或目录
    bool ADmoveFileOrDir(std::string source, std::string target);    
    std::vector<std::string> listFilesInDirectory(const std::string& path); // 列出目录下的所有文件

    // 比较两个目录不同之处
    void compareDirectories(const std::string& dir1, const std::string& dir2);     
};
#endif

