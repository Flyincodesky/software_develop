#include "../include/filetools.hpp"

 
// 打印当前目录树结构
	// @param: dir 当前目录
	// @param: indent 缩进数量
	// @param: depth 递归深度
bool FileTools::printDir(std::string dir, int indent , int depth){
	if(depth<0){
		printf("%*s...\n", indent, "" );
		return true ;
	}
	DIR *dp;
	struct dirent *entry;  //定义在<dirent.h>中，用于表示目录项（即目录中的一个文件或子目录）。通常与 readdir() 函数一起使用来读取目录中的条目。
	struct stat statbuf;   //定义在 <sys/stat.h> 头文件中，用于表示文件的详细信息,用于获取文件的元数据。与 stat() 函数一起使用，用于获取文件的状态信息。
	std::string dirname;
	// 打开目录并返回存储目录信息的DIR结构体
	if ((dp = opendir(dir.c_str())) == NULL){ //使用 opendir 函数尝试打开指定的目录（dir）。如果失败，则输出错误信息并返回 false。
		fprintf(stderr, "Can`t open directory %s\n", dir.c_str());
		return false;
	}
	// 通过先前的DIR目录结构体读取当前目录的目录文件
	// 返回dirent结构体（entry）保存目录文件的信息
	while ((entry = readdir(dp)) != NULL){ // 使用 readdir 函数逐个读取目录项（文件和子目录），直到没有更多项为止。
		//检查读取到的目录项是否为当前目录 (".") 或上级目录 ("..")。如果是，则跳过这些项。
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
			continue; 
		dirname = dir + '/' + entry->d_name;
		lstat(dirname.c_str(), &statbuf); //使用 lstat() 获取 dirname 的状态信息，结果存储在 statbuf 中。
		if (S_ISDIR(statbuf.st_mode)){ //使用 S_ISDIR 宏检查 statbuf.st_mode 是否表示一个目录。
			printf("%*s%s/\n", indent, "", entry->d_name);
			printDir(dirname, indent + 4,--depth);
		}
		else
			printf("%*s%s\n", indent, "", entry->d_name);
	}
	// closedir()关闭已打开的目录
	closedir(dp);
	return true;
}


// 递归创建新目录，若父目录不存在，从最低级目录递归创建
bool FileTools::recurMakeDir(std::string dirPath, mode_t mode)
{
	char path[PATH_MAX];
	strcpy(path, dirPath.c_str());
	if (dirPath == "." || dirPath == "/"){
		return true;
	}
	if (access(dirPath.c_str(), F_OK) == 0)
	{
		return true;
	}
	else
	{
		recurMakeDir(dirname(path));  //dirname(char *path)用于从路径字符串中提取目录路径部分。定义在<libgen.h>中。其主要作用是获取给定路径中最后一个斜杠 (/) 之前的部分，即文件或目录所属的父目录路径。
		mkdir(dirPath.c_str(), mode);
		return true;
	}
}

// 创建新目录函数，便于后续备份目录
bool FileTools::mkDir(std::string dirPathName, mode_t mode)
{
	if (dirPathName == "." || dirPathName == "/")return true;	
	if (access(dirPathName.c_str(), F_OK) == 0)return true;
	else{
		recurMakeDir(dirPathName);
		return true;
	}
	return false;
}

// 目录判空函数，便于删除和移动目录
bool FileTools::EmptyDir(std::string dirName){
	DIR *dp;
	struct dirent *entry;
	struct stat *buff;
	int num = 0;
	if ((dp = opendir(dirName.c_str())) == NULL){
		fprintf(stderr, "Can`t open directory %s\n", dirName.c_str());
		return 0;
	}
	while ((entry = readdir(dp)) != NULL){
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		num++;
	}
	if (num == 0)
		return true;
	else
		return false;
}




/*
// 递归复制目录函数，便于备份目录时调用
// 此处sourceDir相当于子目录，targetDir相当于父目录
bool FileTools::backupDir(std::string sourceDir, std::string targetDir){
	DIR *srcDir;
	struct dirent *srcDirEntry;
	struct stat srcBuff;
	std::string srcPath = sourceDir;
	std::string tgtPath = targetDir;
	std::string filename;
	char srcpath[PATH_MAX], tgtpath[PATH_MAX];
	struct timespec times[2]; // 存储源目录的时间信息

	strcpy(srcpath, srcPath.c_str());
	// 在目标目录下创建同名子目录，并转换到同名子目录下，此时srcPath与tgtPath同级
	tgtPath = targetDir + '/' + basename(srcpath);
	// 判断目标父目录下是否已有重目录，若有则须重命名
	strcpy(tgtpath, sourceDir.c_str());
	while (access(tgtPath.c_str(), F_OK) == 0)
	{
		std::cout << "当前目标目录" << targetDir << "下已有同名文件或目录，请为备份后的目录重命名\n";
		getline(std::cin, tgtPath);
		tgtPath = targetDir + '/' + tgtPath;
	}
	mkDir(tgtPath, srcBuff.st_mode);
	if ((srcDir = opendir(sourceDir.c_str())) == NULL)
	{
		// 打开目录失败，返回失败信息
		fprintf(stderr, "无法打开源目录，请检查后重试 %s\n", targetDir.c_str());
		return false;
	}
	while ((srcDirEntry = readdir(srcDir)) != NULL)
	{
		filename = srcPath + '/' + srcDirEntry->d_name;
		lstat(filename.c_str(), &srcBuff);
		if (strcmp(srcDirEntry->d_name, ".") == 0 || strcmp(srcDirEntry->d_name, "..") == 0)
			continue;
		if (S_ISDIR(srcBuff.st_mode))
		{
			std::cout << filename << std::endl;
			std::cout << tgtPath << std::endl;
			if (!backupDir(filename, tgtPath))
			{
				std::cout << "递归复制失败" << std::endl;
				return false;
			}
		}
		else if (S_ISFIFO(srcBuff.st_mode) || S_ISLNK(srcBuff.st_mode) || S_ISREG(srcBuff.st_mode))
		{
			if (!backupRegFile(filename, tgtPath + '/' + srcDirEntry->d_name))
			{
				std::cout << "复制文件失败" << std::endl;
				return false;
			}
		}
		else
		{
			std::cout << "该文件不是本系统支持的文件类型，不予复制\n";
		}
	}
	closedir(srcDir);
	// 修该备份后的目录文件元数据
	stat(sourceDir.c_str(), &srcBuff);
	times[0] = srcBuff.st_atim;
	times[1] = srcBuff.st_mtim;
	chmod(tgtPath.c_str(), srcBuff.st_mode);
	chown(tgtPath.c_str(), srcBuff.st_uid, srcBuff.st_gid);
	utimensat(AT_FDCWD, tgtPath.c_str(), times, 0);
	return true;
}
 
*/

 
// 删除目录函数，执行删除目录或者文件(兼容)，若目录非空会询问是否删除目录内文件
bool FileTools::rmDirOrFile(std::string tgtName, bool ifconfirm){
	DIR *tgtDP;
	struct dirent *tgtEntry;
	struct stat tgtBuff;
	std::string filename = tgtName;
	lstat(tgtName.c_str(), &tgtBuff);
	// 读取tgtName文件的具体信息，若是空目录或普通文件直接删除，若是目录递归删除
	if (S_ISDIR(tgtBuff.st_mode)){
		if (((tgtDP = opendir(tgtName.c_str())) == NULL)){
			// 打开目录失败，返回失败信息
			fprintf(stderr, "无法打开目标目录，请检查后重试 %s\n", tgtName.c_str());
			return false;
		}else if (EmptyDir(filename)){	// 若目录为空，直接删除，否则进入对目录内文件删除的确认
			rmdir(filename.c_str());
			closedir(tgtDP);
			return true;
		}else{	// 目录非空，开始递归删除目录内文件或子目录
			while ((tgtEntry = readdir(tgtDP)) != NULL){
				if (strcmp(tgtEntry->d_name, ".") == 0 || strcmp(tgtEntry->d_name, "..") == 0)
					continue;
				// 保存递归路径，便于删除
				filename = tgtName + '/' + tgtEntry->d_name;
				lstat(filename.c_str(), &tgtBuff);
				// 若是子目录，进入递归删除
				if (S_ISDIR(tgtBuff.st_mode)){
					rmDirOrFile(filename);
				}
				// 若是目录下普通文件，确认是否删除
				else if (S_ISFIFO(tgtBuff.st_mode) || S_ISLNK(tgtBuff.st_mode) || S_ISREG(tgtBuff.st_mode)){
					char flag;
					if(ifconfirm){
						std::cout << "确定删除当前文件？"<< filename<< "输入y/Y删除,任意其他键跳过删除" << std::endl;
						std::cin >> flag;
						if (flag == 'y' || flag == 'Y'){
							remove(filename.c_str());
						}
					}else	remove(filename.c_str());	
				}
				else	std::cout << "抱歉，当前文件不是本功能支持的文件类型，不予删除\n";
			
			}
			closedir(tgtDP);
		}
		if (EmptyDir(tgtName)){
			rmdir(tgtName.c_str());
			return true;
		}
		else{
			std::cout << "当前目录" << tgtName << "非空，无法删除！\n";
			return false;
		}
	}
	else if (S_ISFIFO(tgtBuff.st_mode) || S_ISLNK(tgtBuff.st_mode) || S_ISREG(tgtBuff.st_mode)){
		char flag;
		if(!ifconfirm){
			remove(filename.c_str());
			return true;
		}
		std::cout << "确定删除当前文件？\n"
			 << filename << std::endl
			 << "输入y/Y删除,任意其他键跳过删除\n";
		std::cin >> flag;
		if (flag == 'y' || flag == 'Y'){
			remove(filename.c_str());
			return true;
		}
		return false;
	}
	return true;
}
 
 


// 列出目录中的文件列表的函数
// std::vector<std::std::string> listFilesInDirectory(const std::std::string &path)
// {
// 	std::vector<std::std::string> files;
// 	for (const auto &entry : std::filesystem::directory_iterator(path))
// 	{
// 		files.push_back(entry.path());
// 	}
// 	return files;
// }


