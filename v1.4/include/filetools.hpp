#ifndef FILETOOLS_HPP
#define FILETOOLS_HPP

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <errno.h>

#include <optional>
#include <random>
#include <time.h>
#include <string.h>
#include <string>
#include <bitset>
#include <vector>
#include <memory>
#include <queue>
#include <map>

#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

#define BUFFER_SIZE 1024

#define COPYINCR (1024 * 1024 * 1024)


// 默认的备份文件存储路径
extern std::string DefaultBackupPath ;

// 记录不同文件的备份信息，用于区分每次备份的是不是已经备份过，以及区分不同文件的备份
extern std::string DefaultBackupInfo ;

// 每个文件备份文件夹里的记录，用于记录每次备份的信息
extern std::string DefaultBackupRecord ;

class FileTools{
 public:
	FileTools(){};  
	~FileTools(){};  

	// 展示当前工作目录结构
	static bool printDir(std::string workdir,int indent , int depth=2);  
	// 对目录判空，便于rmDirOrFile调用
	static bool EmptyDir(std::string dirName);					  
	//bool moveFile(std::string sourceFile, std::string targetDir);	   // 移动文件到指定目录，便于backupDir调用
	// 递归创建新目录，便于mkDir调用
	static bool recurMakeDir(std::string dirPath, mode_t mode_t = 0755); 
	// 创建新目录，默认权限755，便于backupDir调用
	static bool mkDir(std::string dirPathName, mode_t mode = 0755);	
	//bool moveDir(std::string sourceDir, std::string targetDir); // 移动目录到指定目录下
	// 删除目录或者文件
	static bool rmDirOrFile(std::string dirPathName  , bool ifconfirm = true);			  



	// 获取系统时间
	static std::string getCurrentTimeString();
	// 生成指定长度的随机字符串
    static std::string getRandomString(size_t length = 6) ;
	// 获取指定文件（普通文件）的内容行数，不做文件类型检查，因为用于备份记录，一定是普通文件
    static int getFileLine(std::string filepath);
	// 将data追加到path文件(普通文件)的下一行  将新备份的文件info追加入 BackUpInfo 
    static bool addToFile(std::string path , std::string data);


	// 文件复制 从source复制到targetfile
	static bool copyRegFile(std::string source, std::string destination); // 备份普通文件，便于moveFile以及backupDir调用
    static bool copySymLINK(std::string sourcefile, std::string targetfile); // 备份软链接(符号链接)
    static bool copyHardLINK(std::string  sourceFile, std::string targetFile); // 备份硬链接文件 
    static bool copyFIFO(std::string sourcefile, std::string targetfile); // 备份管道
    static bool copyDir(std::string sourceDir, std::string targetDir);  // 包含四种文件类型的目录备份功能

	// 文件复制方法，将 sourcefile 的文件复制到 targetfile 
    static bool copyAllFileKinds(std::string sourcefile, std::string targetfile);


};

class BackUpInfo{
 public:
	//文件名////文件inode(st_ino)////文件设备号////源路径///备份后的文件夹别名
	std::string filename ; 
	std::string inode ;
	std::string devno ;
	std::string path ;
	std::string BURename ; 

	BackUpInfo( std::string filename ,std::string inode , std::string devno , std::string path, std::string BURename) : 
		filename(filename), inode(inode), devno(devno), path(path) , BURename(BURename)  {}
	BackUpInfo( std::string filename ,std::string inode , std::string devno , std::string path ) : 
		filename(filename), inode(inode), devno(devno), path(path){}
	BackUpInfo( std::string line );
	BackUpInfo(){}
	std::vector<std::string> split(const std::string& str, const std::string& delimiter);

	std::string to_string();

	// 自定义的比较函数
    bool isSameFile(const BackUpInfo& other) const ;

std::unique_ptr<BackUpInfo> findSameInVec( std::vector<std::unique_ptr<BackUpInfo>> vec) ;

};

#endif