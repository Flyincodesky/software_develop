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

// using namespace std;

class FileTools{
 public:
	FileTools(){};  
	~FileTools(){};  

	// 展示当前工作目录结构
	bool printDir(std::string workdir,int indent , int depth=2);  
	
	// 对目录判空，便于rmDirOrFile调用
	bool EmptyDir(std::string dirName);					  
	//bool moveFile(std::string sourceFile, std::string targetDir);	   // 移动文件到指定目录，便于backupDir调用
	
	// 递归创建新目录，便于mkDir调用
	bool recurMakeDir(std::string dirPath, mode_t mode_t = 0755); 
	// 创建新目录，默认权限755，便于backupDir调用
	bool mkDir(std::string dirPathName, mode_t mode = 0755);	
	//bool moveDir(std::string sourceDir, std::string targetDir); // 移动目录到指定目录下
	
	// 删除目录或者文件
	bool rmDirOrFile(std::string dirPathName  , bool ifconfirm = true);			  

	// 获取系统时间
	static std::string getCurrentTimeString();
	

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