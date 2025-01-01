#ifndef BACKUPER_HPP
#define BACKUPER_HPP
#include "../AllQtHead.hpp"

#include "../filetools.hpp"

// 默认的备份文件存储路径
extern std::string DefaultBackupPath ;

// 记录不同文件的备份信息，用于区分每次备份的是不是已经备份过，以及区分不同文件的备份
extern std::string DefaultBackupInfo ;

// 每个文件备份文件夹里的记录，用于记录每次备份的信息
extern std::string DefaultBackupRecord ;


class Backuper : public FileTools{
 public:
    Backuper(){};
    ~Backuper(){};

    // 生成指定长度的随机字符串
    std::string getRandomString(size_t length = 6) ;
    // 读取备份存储目录的 BackUpInfo 文件 ,并返回以及备份的文件信息列表
    std::optional<std::vector< std::unique_ptr<BackUpInfo> > >  readBUInfo(std::string path);
    // 将新备份的文件info追加入 BackUpInfo
    bool addToFile(std::string path , std::string data);
    // 获取指定文件的内容行数，不做文件类型检查，因为用于备份记录，一定是普通文件
    int getFileLine(std::string filepath);
    // 备份文件, 备份到默认路径 DefaultBackupPath 
    bool backupFile(std::string sourceFile  ) ;
    // 具体备份文件的方法，将 sourcefile 的文件备份到 targetfile 
    bool backupAllFileKinds(std::string sourcefile, std::string targetfile);



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

