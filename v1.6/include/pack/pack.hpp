#ifndef PACK_HPP
#define PACK_HPP

#include "../backuper/backuper.hpp"

#define BLOCKSIZE 512

typedef struct{
    char name[100];
    char mode[32];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime_sec[16];
    char mtime_nsec[16];
    char typeflag;
    char linkname[100];
    char fileflag;
    char padding[218];
} headblock;

class Packer : public Backuper{
 public:
    Packer(){};
    ~Packer(){};
    long long int getDirSize(std::string sourcedir);       // 获取目录大小，便于计入头结点
    bool packDir(std::string sourcedir, std::string targetbag); // 打包接口函数
    bool addFileToBag(std::string filename, int bag);      // 递归向打包文件写入文件
    headblock *genHeader(std::string sourcefile);          // 在每次添加文件前为文件生成头结点

    bool unpackBag(std::string sourcebag, std::string targetdir); // 解包

    bool turnBagToFile(int sourcebag, std::string targetdir); // 递归拆包
};

#endif