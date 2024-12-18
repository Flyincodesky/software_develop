#include "../../include/backuper/backuper.hpp"
 


// 备份器的普通文件复制函数实现,sourcefile为源文件名，targetfile为目标文件名
bool Backuper::backupRegFile(std::string sourceFile, std::string targetFile){

	int fin, fout;			  // 文件描述符
	void *src, *dst;		  // 无类型指针，便于后续使用
	size_t copysz;			  // 复制的长度，限定在1GB
	struct stat sbuf;		  // 存储源文件具体信息的结构体
	off_t fsz = 0;			  // 复制的初始位置(文件大小)
	struct timespec times[2]; // 存储源文件的时间信息

	// 若以只读方式打开源文件失败则返回
	if ((fin = open(sourceFile.c_str(), O_RDONLY)) < 0){  //O_RDONLY：以只读方式打开文件,定义在<fcntl.h>中
		return false;
	}
	// 用 fstat() 获取源文件的状态信息并存储在 sbuf 结构体中。若获取源文件状态信息失败，关闭文件并返回
	if (fstat(fin, &sbuf) < 0){
		close(fin);
		return false;
	}
	// 以读写模式(O_RDWR)创建(O_CREAT)/打开目标文件，并设置初始权限为 0644(创建文件支持携带路径) , 若创建目标文件失败直接返回 
	// 注意此处采用 O_TRUNC , 如果目标文件存在，将会被覆盖
	if ((fout = open(targetFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0){
		close(fin);// 退出要关闭文件描述符
		return false;
	}
	// 使用 ftruncate() 为目标文件分配与源文件相同的空间大小。 若无法给目标文件分配与源文件同样大小的空间则关闭文件并返回
	if (ftruncate(fout, sbuf.st_size) < 0){
		close(fin);
		close(fout);
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
			return false;
		}
		if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fout, fsz)) == MAP_FAILED){
			close(fin);
			close(fout);
			return false;
		}
		
		// 完成内存映射后直接复制
		memcpy(dst, src, copysz); 
		munmap(src, copysz);	  // 复制完成，撤销内存映射
		munmap(dst, copysz);

		// 修该备份后的元数据, 同步源文件与目标文件的元数据信息，包括访问时间、修改时间、文件权限以及文件所有者等。
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

// 备份软链接文件，sourcefile为源文件名，targetfile为目标文件名
// 可以通过 ln -s 命令创建软连接。例如：ln -s /path/to/target /path/to/link
bool Backuper::backupSymLINK(std::string sourcefile, std::string targetfile){
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

// 备份硬链接文件，sourcefile为源文件名，targetfile为目标文件名
//可以通过 ln（不带 -s 选项）创建硬链接。例如： ln /path/to/target /path/to/link
bool Backuper::backupHardLINK( std::string  sourceFile,   std::string  targetFile) {
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

// 备份管道文件，sourcefile为源文件名，targetfile为目标文件名
bool Backuper::backupFIFO(std::string sourcefile, std::string targetfile){
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

// 递归备份目录，sourceDir为源目录，targetDir为目标父目录
bool Backuper::backupDir(std::string sourceDir, std::string targetDir){
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

    while (access(tgtPath.c_str(), F_OK) == 0){
        std::cout << "当前目标目录" << targetDir << "下已有同名文件或目录，请为备份后的目录重命名"<<std::endl;
        getline(std::cin, tgtPath);
        tgtPath = targetDir + '/' + tgtPath;
    }
    
    mkDir(tgtPath, srcBuff.st_mode);
    if ((srcDir = opendir(sourceDir.c_str())) == NULL){
        // 打开目录失败，返回失败信息
        fprintf(stderr, "无法打开源目录，请检查后重试 %s\n", targetDir.c_str());
        return false;
    }
    // 循环备份目录
    while ((srcDirEntry = readdir(srcDir)) != NULL){
        filename = srcPath + '/' + srcDirEntry->d_name;
        lstat(filename.c_str(), &srcBuff);
        if (strcmp(srcDirEntry->d_name, ".") == 0 || strcmp(srcDirEntry->d_name, "..") == 0)
            continue;
        if (S_ISDIR(srcBuff.st_mode)){ //子目录是一个文件夹，递归调用复制
            if (!backupDir(filename, tgtPath)){
                std::cout << "递归复制失败" << std::endl;
                return false;
            }
        }else if (S_ISREG(srcBuff.st_mode)){ //子目录是一个普通文件
            if (!backupRegFile(filename, tgtPath + '/' + srcDirEntry->d_name)){
                std::cout << "复制文件失败" << std::endl;
                return false;
            }
        }else if (S_ISFIFO(srcBuff.st_mode)){ //子目录是一个管道文件
            if (!backupFIFO(filename, tgtPath + '/' + srcDirEntry->d_name)){
                std::cout << "复制文件失败" << std::endl;
                return false;
            }
        }else if (S_ISLNK(srcBuff.st_mode)){ //子目录是一个软链接文件
            if (!backupSymLINK(filename, tgtPath + '/' + srcDirEntry->d_name)){
                std::cout << "复制文件失败" << std::endl;
                return false;
            }
        }else{
            std::cout << "该文件不是本系统支持的文件类型，不予复制" << std::endl;
        }
    }
    closedir(srcDir);
    // 基于原始数据修该备份后的目录文件元数据
    stat(sourceDir.c_str(), &srcBuff);
    times[0] = srcBuff.st_atim;
    times[1] = srcBuff.st_mtim;
    chmod(tgtPath.c_str(), srcBuff.st_mode);
    chown(tgtPath.c_str(), srcBuff.st_uid, srcBuff.st_gid);
    utimensat(AT_FDCWD, tgtPath.c_str(), times, 0);
    return true;
}

// 兼容性备份，sourcefile为源文件或目录，targetfile为目标父目录
bool Backuper::backupAllFile(std::string sourcefile, std::string targetdir){
    struct stat srcbuf;
    char path[PATH_MAX];
    strcpy(path, sourcefile.c_str());
    std::string targetfilename = targetdir + '/' + basename(path);
    stat(sourcefile.c_str(), &srcbuf);
    if (S_ISDIR(srcbuf.st_mode)){ //目录文件
        if (backupDir(sourcefile, targetdir)){
            std::cout << sourcefile << "备份成功！"<<std::endl;
            return true;
        }
    }else if (S_ISREG(srcbuf.st_mode)){     // 普通文件
        if (backupRegFile(sourcefile, targetfilename)){
            std::cout << sourcefile << "备份成功！"<<std::endl;
            return true;
        }
    }else if (S_ISLNK(srcbuf.st_mode)){     //软链接文件
        if (backupSymLINK(sourcefile, targetfilename)){
            std::cout << sourcefile << "备份成功！"<<std::endl;
            return true;
        }
    }else if (S_ISFIFO(srcbuf.st_mode)){    //管道文件
        if (backupFIFO(sourcefile, targetfilename)){
            std::cout << sourcefile << "备份成功！"<<std::endl;
            return true;
        }
    }else   std::cout << "非常抱歉！当前系统不支持该文件类型"<<std::endl;
    
    return false;
}



// 支持特殊文件类型的文件或目录移动函数
bool Backuper::ADmoveFileOrDir(std::string source, std::string target){
    if (backupAllFile(source, target)){
        std::cout << "备份成功!"<<std::endl;
        if (rmDirOrFile(source , false)){
            std::cout << "移动成功"<<std::endl;
            return true;
        }
    }
    else    std::cout << "移动失败！"<<std::endl;

    return false;
}



// 列举给定路径下的文件(包括目录和具体文件)
std::vector<std::string> Backuper::listFilesInDirectory(const std::string& path) {
    std::vector<std::string> files;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            std::string fileName(entry->d_name);
            if (fileName != "." && fileName != "..") {
                files.push_back(fileName);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }

    return files;
}


// 比较两个目录中的文件列表的函数
void Backuper::compareDirectories(const std::string& dir1, const std::string& dir2) {
    std::vector<std::string> files1 = listFilesInDirectory(dir1);
    std::vector<std::string> files2 = listFilesInDirectory(dir2);
    
    
    std::cout << "Files in " << dir1 << " but not in " << dir2 << ":"<<std::endl;
    for (const std::string& file : files1) {
        std::string fileName;
        if (file.size() > dir1.size()) {
            fileName = file.substr(dir1.size());
        } else {
            fileName = file;
        }
        // std::string fileName = file.substr(dir1.length());
        if (std::find(files2.begin(), files2.end(), dir2 + fileName) == files2.end()) {
            //在dir2里没找到fileName
            std::cout << fileName << std::endl;
        }
    }

    std::cout << "Files in " << dir2 << " but not in " << dir1 << ":"<<std::endl;
    for (const std::string& file : files2) {
        std::string fileName;
        if (file.size() > dir1.size()) {
            fileName = file.substr(dir1.size());
        } else {
            fileName = file;
        }
        // std::string fileName = file.substr(dir2.length());
        if (std::find(files1.begin(), files1.end(), dir1 + fileName) == files1.end()) {
            std::cout << fileName << std::endl;
        }
    }
}

 







