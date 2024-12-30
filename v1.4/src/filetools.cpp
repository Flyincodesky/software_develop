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

// 创建新目录函数，便于后续复制目录
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
 
 









// 获取系统时间
std::string FileTools:: getCurrentTimeString() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();
    
    // 转换为 time_t 类型的时间（即日历时间）
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    // 转换为本地时间并格式化
    std::tm localTime = *std::localtime(&now_time);
    
    // 使用 stringstream 进行格式化
    std::ostringstream timeStream;
    timeStream << std::put_time(&localTime, "%Y-%m-%d-%H:%M:%S"); // 年-月-日-时-分
    return timeStream.str();
}

// 生成指定长度的随机字符串
std::string  FileTools:: getRandomString(size_t length) {
    const char characters[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result;
    result.reserve(length);

    // 使用随机数生成器
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<size_t> distribution(0, sizeof(characters) - 2); // -2 to ignore null terminator

    for (size_t i = 0; i < length; ++i) {
        result += characters[distribution(generator)];
    }
    return result;
}

// 获取指定文件的内容行数，不做文件类型检查，因为用于备份记录，一定是普通文件
int FileTools::getFileLine(std::string filepath){
    std::ifstream inputFile(filepath);
    if (!inputFile.is_open()) {
        return -1; 
    }
    std::string line ;
    int lineNum=0;
    while (std::getline(inputFile,line ))lineNum++;
    inputFile.close();
    return  lineNum;
}

// 将data追加到path文件(普通文件)的下一行  将新备份的文件info追加入 BackUpInfo 
bool FileTools::addToFile(std::string path , std::string data){
    // 创建输出文件流并以追加模式打开
    std::ofstream outFile(path, std::ios::app);
    if (!outFile) {
        std::cerr << "Error opening file: " << path << std::endl;
        return false;
    }
    // 追加数据并换行
    outFile << data << std::endl;
    // 文件流关闭
    outFile.flush(); // 刷新输出流
    outFile.close();
    return true;
}



// 复制器的普通文件复制函数实现,sourcefile为源文件名，targetfile为目标文件名
bool FileTools::copyRegFile(std::string sourceFile, std::string targetFile){

	int fin, fout;			  // 文件描述符
	void *src, *dst;		  // 无类型指针，便于后续使用
	size_t copysz;			  // 复制的长度，限定在1GB
	struct stat sbuf;		  // 存储源文件具体信息的结构体
	off_t fsz = 0;			  // 复制的初始位置(文件大小)
	struct timespec times[2]; // 存储源文件的时间信息

	// 若以只读方式打开源文件失败则返回
	if ((fin = open(sourceFile.c_str(), O_RDONLY)) < 0){  //O_RDONLY：以只读方式打开文件,定义在<fcntl.h>中
        std::cout<<"打开源文件失败"<<std::endl;
        return false;
	}
	// 用 fstat() 获取源文件的状态信息并存储在 sbuf 结构体中。若获取源文件状态信息失败，关闭文件并返回
	if (fstat(fin, &sbuf) < 0){
		close(fin);
        std::cout<<"获取源文件状态失败"<<std::endl;
		return false;
	}
	// 以读写模式(O_RDWR)创建(O_CREAT)/打开目标文件，并设置初始权限为 0644(创建文件支持携带路径) , 若创建目标文件失败直接返回 
	// 注意此处采用 O_TRUNC , 如果目标文件存在，将会被覆盖
	if ((fout = open(targetFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0){
		close(fin);// 退出要关闭文件描述符
        std::cout<<"打开复制存储目标文件失败"<<std::endl;
		return false;
	}
	// 使用 ftruncate() 为目标文件分配与源文件相同的空间大小。 若无法给目标文件分配与源文件同样大小的空间则关闭文件并返回
	if (ftruncate(fout, sbuf.st_size) < 0){
		close(fin);
		close(fout);
        std::cout<<"为目标文件分配与源文件相同的空间大小失败"<<std::endl;
		return false;
	}
	// 通过内存映射的方式实现文件复制，每次只最多复制1GB
	while (fsz < sbuf.st_size){
		// 当复制的大小大于1GB时，限定映射内存1GB, 即每次内存复制仅1GB
		if ((sbuf.st_size - fsz) > COPYINCR) // COPYINCR == 1GB
			copysz = COPYINCR;
		else
			copysz = sbuf.st_size - fsz;
		// 通过内存映射 (mmap) 来将源文件和目标文件的部分内容映射到内存，并通过 memcpy 进行复制。
		// mmap 是一个内存映射函数，用来将文件的内容映射到进程的虚拟内存空间中。通过这种方式，可以直接操作内存来读写文件，避免传统文件I/O中频繁的系统调用，从而提高性能。
		if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fin, fsz)) == MAP_FAILED){  //如果 mmap 返回 MAP_FAILED，说明内存映射失败
			// 将源文件 fin 的一部分(大小为 copysz)从偏移量fsz开始，映射到内存中，并将这个映射区域的起始地址赋给 src。
				// 0: 指定内存映射的起始地址。传入 0 表示由内核自动选择合适的地址。// copysz: 映射的内存大小(字节)。
				// PROT_READ: 表示内存区域的权限，这里是只读权限。// MAP_SHARED: 表示映射的内存区域是共享的，意味着对该内存区域的修改会反映到文件中。
				// fin: 源文件的文件描述符。// fsz: 映射的起始偏移量，表示从源文件的这个位置开始映射。
			close(fin);
			close(fout);
            std::cout<<"将源文件映射到虚拟内存失败"<<std::endl;
			return false;
		}
		if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fout, fsz)) == MAP_FAILED){
			close(fin);
			close(fout);
            std::cout<<"将目标文件映射到虚拟内存失败"<<std::endl;
			return false;
		}
		
		// 完成内存映射后直接复制
		memcpy(dst, src, copysz); 
		munmap(src, copysz);	  // 复制完成，撤销内存映射
		munmap(dst, copysz);

		// 修该复制后的元数据, 同步源文件与目标文件的元数据信息，包括访问时间、修改时间、文件权限以及文件所有者等。
		times[0] = sbuf.st_atim;  // 访问时间, 即文件上次被读取的时间。
		times[1] = sbuf.st_mtim;  // 修改时间,即文件内容上次被修改的时间。
		fchmod(fout, sbuf.st_mode); // 设置目标文件的权限
		chown(targetFile.c_str(), sbuf.st_uid, sbuf.st_gid);  // 设置文件拥有者和组, 将目标文件的用户ID（st_uid）和组ID（st_gid）设置为与源文件相同。
		futimens(fout, times); //使用 futimens() 函数，将目标文件的访问时间和修改时间设置为源文件的时间。
		// 文件复制首字节偏移复制量
		fsz += copysz;
	}
	// 退出要关闭文件描述符
	close(fin);
	close(fout);
	return true;
}

// 复制软链接文件，sourcefile为源文件名，targetfile为目标文件名
// 可以通过 ln -s 命令创建软连接。例如：ln -s /path/to/target /path/to/link
bool FileTools::copySymLINK(std::string sourcefile, std::string targetfile){
    char buf[BUFFER_SIZE];  // 缓冲区，用于存储符号链接指向的目标路径
    struct timespec times[2];
    struct stat statbuf;
    if (stat(sourcefile.c_str(), &statbuf) != 0){ //stat()获取 sourcefile 状态信息，结果存储在 statbuf 中。返回0表示成功，返回非0表示失败。
        perror("link");
        return false;
    }
    // 读取源符号链接的目标路径
    readlink(sourcefile.c_str(), buf, sizeof(buf)); // 读取源链接:readlink()读取符号链接 sourcefile 指向的目标路径，并将其存储在 buf 中,该函数不在 buf 字符数组末尾添加空字符
    // 创建符号链接
    if (symlink(buf, targetfile.c_str()) < 0) {   // 复制源链接:symlink()在目标位置 targetfile 处创建一个新的符号链接，指向源文件的目标路径（即 buf 中存储的路径）。 
        perror("link");
        return false;
    }

    // 保留元数据
    times[0] = statbuf.st_atim;
    times[1] = statbuf.st_mtim;
    chown(targetfile.c_str(), statbuf.st_uid, statbuf.st_gid);
    utimensat(AT_FDCWD, targetfile.c_str(), times, AT_SYMLINK_NOFOLLOW); //将目标符号链接的访问时间和修改时间设置为源文件的时间。

    return true;
}

// 复制硬链接文件，sourcefile为源文件名，targetfile为目标文件名
//可以通过 ln（不带 -s 选项）创建硬链接。例如： ln /path/to/target /path/to/link
bool FileTools::copyHardLINK( std::string  sourceFile,   std::string  targetFile) {
    struct timespec times[2];
    struct stat statbuf;
    // 获取源文件的状态信息
    if (stat(sourceFile.c_str(), &statbuf) != 0) {
        perror("Failed to get file status");
        return false;
    }
    // 创建硬链接
    if (link(sourceFile.c_str(), targetFile.c_str()) != 0) {
        std::cerr << "Failed to create hard link: " << strerror(errno) << std::endl;
        return false;
    }

    // 保留源文件的元数据
    times[0] = statbuf.st_atim; // 访问时间
    times[1] = statbuf.st_mtim; // 修改时间
    chmod(targetFile.c_str(), statbuf.st_mode)   ;   // 保留权限
    chown(targetFile.c_str(), statbuf.st_uid, statbuf.st_gid);  // 保留文件拥有者和组
    utimensat(AT_FDCWD, targetFile.c_str(), times, AT_SYMLINK_NOFOLLOW) ;// 设置文件的时间戳信息（访问时间和修改时间）

    return true;
}

// 复制管道文件，sourcefile为源文件名，targetfile为目标文件名
bool FileTools::copyFIFO(std::string sourcefile, std::string targetfile){
    struct stat statbuf;
    struct timespec times[2];
    if (stat(sourcefile.c_str(), &statbuf)){ //stat() 函数获取源文件的信息
        perror("pipe");
        return false;
    } 
    if (mkfifo(targetfile.c_str(), statbuf.st_mode) != 0){ //使用 mkfifo() 函数创建目标 FIFO 文件，权限与源文件相同（通过 statbuf.st_mode 获取）。
        perror("pipe");
        return false;
    }

    // 保留元数据
    times[0] = statbuf.st_atim;
    times[1] = statbuf.st_mtim;
    chown(targetfile.c_str(), statbuf.st_uid, statbuf.st_gid);
    utimensat(AT_FDCWD, targetfile.c_str(), times, AT_SYMLINK_NOFOLLOW);

    return true;
}

// 递归复制目录，sourceDir为源目录，targetDir为目标目录
bool FileTools::copyDir(std::string sourceDir, std::string targetDir){
    DIR *srcDir;
    struct dirent *srcDirEntry;
    struct stat srcBuff;
    std::string filename;
    struct timespec times[2]; // 存储源目录的时间信息
    
    mkDir(targetDir, srcBuff.st_mode);
    if ((srcDir = opendir(sourceDir.c_str())) == NULL){
        // 打开目录失败，返回失败信息
        fprintf(stderr, "无法打开源目录，请检查后重试 %s\n", targetDir.c_str());
        return false;
    }
    // 循环复制目录
    while ((srcDirEntry = readdir(srcDir)) != NULL){
        filename = sourceDir + '/' + srcDirEntry->d_name;
        lstat(filename.c_str(), &srcBuff);
        if (strcmp(srcDirEntry->d_name, ".") == 0 || strcmp(srcDirEntry->d_name, "..") == 0)
            continue;
        if (S_ISDIR(srcBuff.st_mode)){ //子目录是一个文件夹，递归调用复制
            if (!copyDir(filename, targetDir)){
                std::cout << "递归复制失败" << std::endl;
                return false;
            }
        }else if (S_ISREG(srcBuff.st_mode)){ //子目录是一个普通文件
            if (!copyRegFile(filename, targetDir + '/' + srcDirEntry->d_name)){
                std::cout << "复制文件失败" << std::endl;
                return false;
            }
        }else if (S_ISFIFO(srcBuff.st_mode)){ //子目录是一个管道文件
            if (!copyFIFO(filename, targetDir + '/' + srcDirEntry->d_name)){
                std::cout << "复制文件失败" << std::endl;
                return false;
            }
        }else if (S_ISLNK(srcBuff.st_mode)){ //子目录是一个软链接文件
            if (!copySymLINK(filename, targetDir + '/' + srcDirEntry->d_name)){
                std::cout << "复制文件失败" << std::endl;
                return false;
            }
        }else{
            std::cout << "该文件不是本系统支持的文件类型，不予复制" << std::endl;
        }
    }
    closedir(srcDir);
    // 基于原始数据修该复制后的目录文件元数据
    stat(sourceDir.c_str(), &srcBuff);
    times[0] = srcBuff.st_atim;
    times[1] = srcBuff.st_mtim;
    chmod(targetDir.c_str(), srcBuff.st_mode);
    chown(targetDir.c_str(), srcBuff.st_uid, srcBuff.st_gid);
    utimensat(AT_FDCWD, targetDir.c_str(), times, 0);
    return true;
}

// 文件复制方法，将 sourcefile 的文件复制到 targetfile 
bool  FileTools::copyAllFileKinds(std::string sourcefile, std::string targetfile){
	struct stat srcbuf;
    stat(sourcefile.c_str(), &srcbuf);
    if (S_ISDIR(srcbuf.st_mode)){   //目录文件
        if (copyDir(sourcefile, targetfile)){
            std::cout << sourcefile << "复制成功！"<<std::endl;
            return true;
        }
    }else if (S_ISREG(srcbuf.st_mode)){     // 普通文件
        if (copyRegFile(sourcefile, targetfile)){
            std::cout << sourcefile << "复制成功！"<<std::endl;
            return true;
        }
    }else if (S_ISLNK(srcbuf.st_mode)){     //软链接文件
        if (copySymLINK(sourcefile, targetfile)){
            std::cout << sourcefile << "复制成功！"<<std::endl;
            return true;
        }
    }else if (S_ISFIFO(srcbuf.st_mode)){    //管道文件
        if (copyFIFO(sourcefile, targetfile)){
            std::cout << sourcefile << "复制成功！"<<std::endl;
            return true;
        }
    }else   std::cout << "非常抱歉！当前系统不支持该文件类型"<<std::endl;
    
    return false;

	
}





























// BackUpInfo 的构造函数,根据 BackUpInfo(file) 读取的一行字符串初始化
BackUpInfo::BackUpInfo( std::string line ){
	std::vector<std::string> parts = this->split(line , "////");
	if(parts.size()!=5){
		std::cout<<"BackUpInfo文件读取错误!"<<std::endl;
		exit(0);
	}
	this->filename = parts[0];
	this->inode = parts[1];
	this->devno = parts[2];
	this->path = parts[3];
	this->BURename = parts[4];
}

// 将字符串按照 delimiter 划分
std::vector<std::string> BackUpInfo:: split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start)); // 处理最后一个部分

    return tokens;
}

// 将类属性转化成 BackUpInfo(file) 的一行格式 : //文件名////文件inode(st_ino)////文件设备号////源路径 
std::string BackUpInfo:: to_string(){
	return this->filename+"////"+this->inode+"////"+this->devno+"////"+this->path+"////"+this->BURename ;
}	

bool BackUpInfo:: isSameFile(const BackUpInfo& other) const {
    return (this->inode == other.inode)&&(this->devno == other.devno) ; // 根据需要修改属性比较
}

std::unique_ptr<BackUpInfo> BackUpInfo:: findSameInVec( std::vector<std::unique_ptr<BackUpInfo>> vec) {
	// 迭代
    for (auto it = vec.begin(); it != vec.end();it++) {
        if(this->isSameFile(*it->get())){
			//是同一个文件，返回该ptr
			return std::move(*it);
		}
    }
	return nullptr ;
}
