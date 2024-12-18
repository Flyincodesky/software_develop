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
#include <limits>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>  // 提供 getcwd 函数
#include <sys/types.h>
#include <errno.h>

#include <time.h>
#include <string.h>
#include <string>
#include <bitset>
#include <vector>
#include <queue>
#include <map>

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

};

#endif