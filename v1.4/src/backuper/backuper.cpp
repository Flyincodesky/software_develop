#include "../../include/backuper/backuper.hpp"

// 读取备份存储目录的 BackUpInfo 文件 ,并返回以及备份的文件信息列表
std::optional<std::vector< std::unique_ptr<BackUpInfo> > > Backuper:: readBUInfo(std::string path){
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << path << std::endl;
        // QMessageBox::information(nullptr, "Error", "Unable to open file:"+QString::fromStdString(path)); 
        return std::nullopt; // 表示失败
    }

    std::vector< std::unique_ptr<BackUpInfo> > buInfos ;
    std::string line;
    while (std::getline(inputFile, line)) {
        // 处理每一行
        // std::cout << line << std::endl;
        std::unique_ptr<BackUpInfo> buInfo =std::make_unique<BackUpInfo>();
        std::vector<std::string> parts = buInfo->split(line , "////");
        if(parts.size()!=5){
            std::cout<<"BackUpInfo文件读取错误!"<<std::endl;
            return std::nullopt; // 表示失败
        }
        buInfo->filename = parts[0];
        buInfo->inode = parts[1];
        buInfo->devno = parts[2];
        buInfo->path = parts[3];
        buInfo->BURename = parts[4]; 
        buInfos.push_back(std::move(buInfo));
    }

    inputFile.close(); // 关闭文件

    return buInfos;

}

// 备份文件, 备份到默认路径 DefaultBackupPath 
bool Backuper::backupFile(std::string sourceFile  ) {
    // std::cout<<"sb"+sourceFile+DefaultBackupPath<<std::endl; 
    std::string BUInfoFilePath = DefaultBackupPath + "/BackUpInfo" ;
    //// 判断该文件之前是否备份过,若备份过,在原备份记录上新增备份点
    // 首先获取 sourceFile 的文件信息 , 构建 BackUpInfo 类
    struct stat srcbuf;
    stat(sourceFile.c_str(), &srcbuf);
    std::unique_ptr<BackUpInfo> sourceFileInfo =std::make_unique<BackUpInfo>(sourceFile.substr(sourceFile.find_last_of("/\\") + 1) , 
                                                        std::to_string(srcbuf.st_ino), std::to_string(srcbuf.st_dev) , sourceFile);
    
    // 读取 BUInfoFilePath : /home/cl/Desktop/Backup_King/BackupDir/ + BackUpInfo 内容
    auto resultOpt = Backuper::readBUInfo(BUInfoFilePath) ;
    if(!resultOpt)return false ; 
    std::vector< std::unique_ptr<BackUpInfo> > BUInfos = std::move(*resultOpt) ;
    // 获取 所有已备份文件的备份文件夹重命名
    std::vector<std::string> AllRename ; 
    for (const auto& ptr:BUInfos ) AllRename.push_back(ptr->BURename) ;

    // 查找 sourceFileInfo 是否在 BUInfos 里，即是否备份过
    std::unique_ptr<BackUpInfo> findRecord  = sourceFileInfo->findSameInVec(std::move(BUInfos)) ;
    
    std::string BUFilePath ; // 待备份文件自己的备份存储文件夹
    if (findRecord == nullptr) {  // 没找到，说明该文件是第一次备份
        // BackUpInfo行内容:   文件名////文件inode(st_ino)////文件设备号////源路径////备份重命名
        // 首先在 BackUpInfo 里面写入新的记录，但是此时还有一个属性未定义(UPRename)，在备份目录 DefaultBackupPath 里新开一个自己的备份文件夹 ，这个文件夹名要避免重名
        // 查看本文件的filename在AllRename中有无重名，若有，则需要更改，更改的规则是加在filename文件名后加一个6位的字符串，其内容为字母或数字随机排列
        if(std::find(AllRename.begin() , AllRename.end() , sourceFileInfo->filename )!=AllRename.end()){
            // 有命名冲突
            sourceFileInfo->BURename = sourceFileInfo->filename + FileTools::getRandomString();
        }else sourceFileInfo->BURename = sourceFileInfo->filename ; // 没有命名冲突直接用原名即可
        if(FileTools::addToFile(BUInfoFilePath , sourceFileInfo->to_string())==false){ // 写入失败
            std::cout<<"备份文件信息写入BackUpInfo失败"<<std::endl;
            return false ;
        }
        // BackUpInfo已经写入记录 ， 下面在/home/cl/Desktop/Backup_King/BackupDir/BackUpFiles 里新建其重命名的BURename文件夹
        BUFilePath = DefaultBackupPath + "/BackUpFiles/" + sourceFileInfo->BURename ;
        FileTools filetool ;
        filetool.mkDir(BUFilePath); 
        // 在BUFilePath新建一个备份记录文件 BackUpRecord.hwcq307,用于记录该文件的每次备份
        // 打开文件并使用 trunc 模式清空内容
        std::ofstream outFile(BUFilePath+ "/"+DefaultBackupRecord , std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error creating file: " << BUFilePath + DefaultBackupRecord << std::endl;
            return false;
        }
        outFile.close(); // 关闭文件
        
    } else { // 找到了记录，说明该文件不是第一次备份了 ,在原备份记录上新增备份点
        // std::cout<<"end"<<std::endl;
        BUFilePath = DefaultBackupPath + "/BackUpFiles/" + findRecord->BURename ;
        sourceFileInfo ->BURename = findRecord->BURename;
    }

    // 不管找不找到，到这的都是对 sourceFileInfo 进行备份，备份的路径是 BUFilePath , 但是还要知道当前是第几备份点，要查看 备份记录文件 BackUpRecord.hwcq307
    int recordNum= FileTools::getFileLine(BUFilePath +"/" + DefaultBackupRecord) ;
    if(recordNum<0){ // 读取文件失败
        std::cerr << "Unable to open file: " << BUFilePath + DefaultBackupRecord << std::endl;
        return false; 
    }
    std::string BUVersion = sourceFileInfo ->BURename + "V" + std::to_string(recordNum) ;
    // 先写备份记录文件 其内容为每行是一条备份记录： 备份时的文件名////当前备份的备份文件名（UPRenameVi）////备份时间////备份时的路径
    std::string wdata = sourceFileInfo->filename +"////" + BUVersion+"////" + FileTools::getCurrentTimeString() + "////" +sourceFile  ;
    
    // 下面进行备份
    if(FileTools::copyAllFileKinds(sourceFile  , BUFilePath + "/" +BUVersion )){
        FileTools::addToFile(BUFilePath +"/" + DefaultBackupRecord ,  wdata) ;
        return true;    
    }   
    return false;

}





/*
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
*/
 




