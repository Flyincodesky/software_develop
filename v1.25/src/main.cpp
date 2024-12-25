 
#include "../include/filetools.hpp"
#include "../include/backuper/backuper.hpp"
#include "../include/pack/pack.hpp"
#include "../include/compress/compress.hpp"
#include "../include/encrypt/encrypt.hpp"

std::string defaultWorkPath="/home/cl/Desktop/Backup_King/testFiles" , curWorkPath;

void print_function(){
    std::cout<<"\n当前工作目录为:\""<<curWorkPath<<"\" , 请输入对应数字选择功能：" <<std::endl;
    std::cout
        << "  0：切换工作目录\t1：备份硬链接文件\t2：备份软链接文件\t3：备份管道文件" << std::endl
        << "  4：备份目录    \t5：创建空目录   \t6：备份文件(兼容)\t7：还原文件(兼容)" << std::endl
        << "  8：备份目录对比\t9：目录打包     \t10：目录解包     \t11：压缩文件" << std::endl
        << "  12：解压文件   \t13：加密单个文件\t14：解密文件     \t15：退出系统" << std::endl;
}

int main(){
    curWorkPath = defaultWorkPath;

    int function_flag = 17;
    char source_path[PATH_MAX];
    std::string destination_path;
    std::string dirPathName;
    std::string targetFile;      // 复制文件的目标文件名
    std::string sourceDirOrFile; // 备份或移动的源目录或源文件
    std::string targetDir;       // 移动文件的目标父目录或复制备份目录的父目录
    std::string password;
    std::string Dirpath1; // 用于对比目录
    std::string Dirpath2; // 用于对比目录
    struct stat sbuf;

    std::cout << "欢迎使用本系统，本系统支持文件类型: 1.普通文件; 2.软硬连接; 3.管道文件; 4.目录文件" << std::endl;
  
    if (chdir(curWorkPath.c_str()) != 0) {
        perror("chdir failed");
        exit(0);
    }
    std::cout << "当前目录已更改为:\""<<curWorkPath <<"\"."<< std::endl;
    std::cout << "当前目录的结构如下："<<std::endl;
    FileTools filetool;
    filetool.printDir(".", 0);

    while (function_flag != 15){
        print_function();
        std::cin >> function_flag;
        // 非法输入检查
        if (std::cin.fail() || function_flag < 0 || function_flag > 15) {
            std::cin.clear(); // 清除错误状态
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略缓冲区中剩余的输入
            std::cout << "非法输入，请输入0-15的数字！" << std::endl;
            continue;
        }
        getchar();

        switch (function_flag){
            case 0:{    // 切换工作目录
                std::cout << "请输入想要切换的工作目录(相对路径)：" <<std::endl;
                std::getline(std::cin, dirPathName); //输入切换路径赋值dirPathName
                getcwd(source_path, PATH_MAX); //getcwd() 函数获取当前工作目录的绝对路径，存储在 source_path 数组中（大小为 PATH_MAX，即系统定义的最大路径长度）
                dirPathName = std::string(source_path) + '/' + dirPathName;
                std::cout<<"当前工作路径为：\""<<source_path<<"\""<<" , 待切换的工作路径为：\""<<dirPathName<<"\""<<std::endl;
                if (chdir(dirPathName.c_str()) == 0){
                    getcwd(source_path, PATH_MAX);
                    curWorkPath = std::string(source_path);
                    std::cout << "目录切换成功,当前工作目录为: \""<<curWorkPath<<"\""<<" , 当前目录树如下："<<std::endl;
                    //输出目录树结构
                    filetool.printDir(".", 0);
                }else{
                    std::cout << "切换失败，请检查后重新输入"<<std::endl;
                }
                break;
            }
            case 1:{    // 备份普通文件
                std::cout << "请输入想要备份的文件（请输入相对路径及文件名称）" << std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "文件" << sourceDirOrFile << "不存在，请检查后重新输入"<<std::endl;
                    break;
                }
                // 获取文件状态信息
                lstat(sourceDirOrFile.c_str(), &sbuf);
                // 检查文件状态信息中文件类型，判断本系统是否支持
                if (!(S_ISFIFO(sbuf.st_mode) || S_ISLNK(sbuf.st_mode) || S_ISREG(sbuf.st_mode))){
                    std::cout << "文件" << sourceDirOrFile << "不是本系统支持的文件类型，请检查后重新输入"<<std::endl;
                    break;
                }
                std::cout << "请输入存储备份文件的目录名(相对路径)" << std::endl;
                getline(std::cin, targetDir);
                // 处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path, sourceDirOrFile.c_str());
                targetFile = targetDir + '/' + basename(source_path);
                while (access(targetFile.c_str(), F_OK) == 0){
                    std::cout << "当前目标目录" << targetDir << "下已有同名文件或目录，请为备份后的文件重命名"<<std::endl;
                    getline(std::cin, targetFile);
                    targetFile = targetDir + '/' + targetFile;
                }
                Backuper adworker;
                if(adworker.backupRegFile(sourceDirOrFile, targetFile))std::cout<<basename(const_cast<char * >( targetFile.c_str()))<<" backup success!"<<std::endl;
                else std::cout<<"backup fail"<<std::endl;
                break;
            }
            case 2:{    // 备份链接文件
                std::cout << "请输入想要备份的链接（相对路径以及名称）"<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "链接" << sourceDirOrFile << "不存在，请检查后重新输入"<<std::endl;
                    break;
                }
                // 获取文件状态信息
                lstat(sourceDirOrFile.c_str(), &sbuf);
                if (!S_ISLNK(sbuf.st_mode)){
                    std::cout << "文件" << sourceDirOrFile << "不是链接文件，请检查后重新输入"<<std::endl;
                    break;
                }
                std::cout << "请输入存储备份链接的目录名" << std::endl;
                getline(std::cin, targetDir);
                // 处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path, sourceDirOrFile.c_str());
                targetFile = targetDir + '/' + basename(source_path);
                while (access(targetFile.c_str(), F_OK) == 0){
                    std::cout << "当前目标目录" << targetDir << "下已有同名文件或目录，请为备份后的文件重命名"<<std::endl;
                    getline(std::cin, targetFile);
                    targetFile = targetDir + '/' + targetFile;
                }
                Backuper ADworker;
                ADworker.backupSymLINK(sourceDirOrFile, targetFile);
                break;
            }
            case 3:{    // 备份管道文件 
                std::cout << "请输入想要备份的管道（相对路径以及名称）" << std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "管道" << sourceDirOrFile << "不存在，请检查后重新输入" << std::endl;
                    break;
                }
                lstat(sourceDirOrFile.c_str(), &sbuf);
                // 判断是不是管道文件
                if (!S_ISFIFO(sbuf.st_mode)){
                    std::cout << "文件" << sourceDirOrFile << "不是管道文件，请检查后重新输入" << std::endl;
                    break;
                }
                std::cout << "请输入存储备份管道的目录名" << std::endl;
                getline(std::cin, targetDir);
                // 处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path, sourceDirOrFile.c_str());
                targetFile = targetDir + '/' + basename(source_path);
                while (access(targetFile.c_str(), F_OK) == 0){
                    std::cout << "当前目标目录" << targetDir << "下已有同名文件或目录，请为备份后的文件重命名"<< std::endl;
                    getline(std::cin, targetFile);
                    targetFile = targetDir + '/' + targetFile;
                }

                Backuper ADworker;
                ADworker.backupFIFO(sourceDirOrFile, targetFile);
                break;
            }
            case 4:{    // 备份目录
                std::cout << "请输入想要备份的目录"<< std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "目录" << sourceDirOrFile << "不存在，请检查输入或创建目录后重试"<< std::endl;
                    break;
                }
                stat(sourceDirOrFile.c_str(), &sbuf);
                if (!S_ISDIR(sbuf.st_mode)){  //stat函数获取路径文件信息存储进sbuf, 根据sbuf检查输入的路径是不是一个目录
                    std::cout << "当前输入" << sourceDirOrFile << "不是目录，请检查后重新输入"<< std::endl;
                    break;
                }
                std::cout << "请输入将要储存备份目录的父目录"<< std::endl;
                getline(std::cin, targetDir);
                if (strcmp(sourceDirOrFile.c_str(), targetDir.c_str()) == 0){
                    std::cout << "两目录禁止同名，退出备份！"<< std::endl;
                    break;
                }
                Backuper ADworker;
                // 如果选择移入目录不存在，可直接建立
                stat(targetDir.c_str(), &sbuf);
                if (access(targetDir.c_str(), F_OK) != 0){
                    char flag;
                    std::cout << "当前目标父目录不存在，是否需要直接创建新目录?若是请输入y/Y,其他任意键退出"<< std::endl;                        
                    std::cin >> flag;
                    if (flag == 'y' || flag == 'Y'){
                        ADworker.mkDir(targetDir);
                    }
                    else
                        break;
                }else if (!S_ISDIR(sbuf.st_mode)){ // 输入的路径是一个文件，而不是文件夹
                    std::cout << "当前输入" << targetDir << "已有同名文件，备份失败"<< std::endl;
                    break;
                }
                ADworker.backupDir(sourceDirOrFile, targetDir);
                break;
            }
            case 5:{    // 创建新目录
                std::cout << "请输入想要创建的目录名\n(！不支持绝对路径下创建新目录,但可在绝对路径已有目录下创建新目录)"<< std::endl;
                getline(std::cin, dirPathName);
                if (access(dirPathName.c_str(), F_OK) == 0){
                    std::cout << "当前目录" << dirPathName << "已有同名文件或目录，请检查后重新输入"<<std::endl;
                    break;
                }
                Backuper ADworker;
                if (ADworker.mkDir(dirPathName))
                    std::cout << "创建成功"<<std::endl;
                else
                    std::cout << "创建失败"<<std::endl;
                break;
            }
            case 6:{    // 备份文件（兼容上述四种文件）
                std::cout << "请输入想要备份的文件或目录（相对路径以及名称）"<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "文件或目录" << sourceDirOrFile << "不存在，请检查后重新输入"<<std::endl;
                    break;
                }
                lstat(sourceDirOrFile.c_str(), &sbuf);
                if (!(S_ISFIFO(sbuf.st_mode) || S_ISLNK(sbuf.st_mode) || S_ISREG(sbuf.st_mode) || S_ISDIR(sbuf.st_mode))){
                    std::cout << "文件" << sourceDirOrFile << "类型不在支持范围内，抱歉！"<<std::endl;
                    break;
                }
                std::cout << "请输入存储备份文件或目录的父目录名" << std::endl;
                getline(std::cin, targetDir);
                // 若备份目录时出现同名，退出备份
                if ((strcmp(sourceDirOrFile.c_str(), targetDir.c_str()) == 0) && (S_ISDIR(sbuf.st_mode))){
                    std::cout << "两目录禁止同名，退出备份！"<<std::endl;
                    break;
                }
                // 处理输入的目录以及文件名，判断是否已有重名文件，若有则须重命名
                strcpy(source_path, sourceDirOrFile.c_str());
                targetFile = targetDir + '/' + basename(source_path);
                while (access(targetFile.c_str(), F_OK) == 0){
                    std::cout << "当前目标目录" << targetDir << "下已有同名文件或目录，请为备份后的文件重命名"<<std::endl;
                    getline(std::cin, targetFile);
                    targetFile = targetDir + '/' + targetFile;
                }
                Backuper ADworker;
                ADworker.backupAllFile(sourceDirOrFile, targetDir);
                break;
            }
            ////**** 还原文件
            case 7:{    // 还原文件（兼容性）
                std::cout << "请输入想要移动的文件或目录名"<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "文件" << sourceDirOrFile << "不存在，请检查后重新输入"<<std::endl;
                    break;
                }
                lstat(sourceDirOrFile.c_str(), &sbuf);
                if (!(S_ISFIFO(sbuf.st_mode) || S_ISLNK(sbuf.st_mode) || S_ISREG(sbuf.st_mode) || S_ISDIR(sbuf.st_mode))){
                    std::cout << "文件" << sourceDirOrFile << "类型不在支持范围内，抱歉！"<<std::endl;
                    break;
                }
                std::cout << "请输入想要移入的目标目录"<<std::endl;
                getline(std::cin, targetDir);
                stat(sourceDirOrFile.c_str(), &sbuf);
                // 若移动目录时出现同名，退出移动
                if ((strcmp(sourceDirOrFile.c_str(), targetDir.c_str()) == 0) && (S_ISDIR(sbuf.st_mode))){
                    std::cout << "两目录禁止同名，退出备份！"<<std::endl;
                    break;
                }
                Backuper ADworker;
                ADworker.ADmoveFileOrDir(sourceDirOrFile, targetDir);
                break;
            }        
            case 8:{    // 备份目录对比
                std::cout << "请输入想要比较的目录路径1"<<std::endl;
                std::getline(std::cin, Dirpath1);
                std::cout << "请输入想要比较的目录路径2"<<std::endl;
                std::getline(std::cin, Dirpath2);

                if (access(Dirpath1.c_str(), F_OK)){
                    std::cout << "输入目录" << Dirpath1 << "不存在，请检查后重新输入"<<std::endl;
                    break;
                }
                if (access(Dirpath2.c_str(), F_OK)){
                    std::cout << "输入目录" << Dirpath2 << "不存在，请检查后重新输入"<<std::endl;
                    break;
                }

                // lstat(Dir_path1.c_str(), &sbuf);
                // if (!(S_ISDIR(sbuf.st_mode) || S_ISFIFO(sbuf.st_mode) || S_ISLNK(sbuf.st_mode) || S_ISREG(sbuf.st_mode)))
                // {
                //     cout << "文件或目录" << Dir_path1 << "不是本系统支持的文件类型，请检查后重新输入"<<std::endl;
                //     break;
                // }

                Backuper ADworker;
                ADworker.compareDirectories(Dirpath1, Dirpath2);
                break;
            }
            ////**** 打包解包
            case 9:{    // 打包文件
                std::cout << "请输入要打包的目录："<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK) || filetool.EmptyDir(sourceDirOrFile)){
                    std::cout << "当前目录不存在或当前目录为空，请检查后重新输入"<<std::endl;
                }else{
                    std::cout << "请为打包后的文件命名(自动添加后缀)："<<std::endl;
                    getline(std::cin, targetFile);
                    Packer pack_worker;
                    pack_worker.packDir(sourceDirOrFile, targetFile);
                }
                break;
            }
            case 10:{   // 解包文件
                std::cout << "请输入需要解包的文件(包括路径和后缀)："<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)){
                    std::cout << "当前目录不存在，请检查后重新输入"<<std::endl;
                    break;
                } 
                Packer pack_worker;
                std::cout << "请输入解包所得目录的父目录："<<std::endl;
                getline(std::cin, targetDir);
                if (access(targetDir.c_str(), F_OK)){
                    char flag;
                    std::cout << "当前目标父目录不存在，是否需要直接创建新目录"<<std::endl
                        << "若是请输入y/Y,其他任意键退出"<<std::endl;
                    std::cin >> flag;
                    if (flag == 'y' || flag == 'Y')pack_worker.mkDir(targetDir);
                    else    break;
                }
                pack_worker.unpackBag(sourceDirOrFile, targetDir);
                
                break;
            }
            ////**** 压缩解压
            case 11:{   // 压缩文件
                std::cout << "请输入需要压缩的文件："<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)) std::cout << "当前文件不存在，请检查后后重新输入"<<std::endl;
                else{
                    std::cout << "请为压缩后文件命名(自动添加后缀)："<<std::endl;
                    getline(std::cin, targetFile);
                    Compresser compresser;
                    compresser.compress(sourceDirOrFile, targetFile);
                }
                break;
            }
            case 12:{   // 解压文件
                std::cout << "请输入需要解压的文件(包含后缀)："<<std::endl;
                getline(std::cin, sourceDirOrFile);
                if (access(sourceDirOrFile.c_str(), F_OK)) {
                    std::cout << "当前文件不存在，请检查后后重新输入"<<std::endl;
                }else{
                    std::cout << "请输入存放解压后文件的父目录："<<std::endl;
                    getline(std::cin, targetDir);
                    if (access(targetDir.c_str(), F_OK)){
                        char flag;
                        std::cout << "当前目标父目录不存在，是否需要直接创建新目录"<<std::endl
                            << "若是请输入y/Y,其他任意键退出"<<std::endl;
                        std::cin >> flag;
                        if (flag == 'y' || flag == 'Y') {
                            filetool.mkDir(targetDir);
                        }
                        else  break;
                    }
                    Compresser compresser;
                    if (compresser.decompress(sourceDirOrFile, targetDir)) std::cout << "解压成功!"<<std::endl;
                }
                break;
            }
             
            
            default:{
                std::cout<< "输入错误，请重新输入！"<< std::endl;
                break;
            }
            std::cout << "按任意键继续使用"<<std::endl;
        }

    
    }

    return 0;
}