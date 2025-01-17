#include "../../include/compress/compress.hpp"

 
// 编码函数
void Compresser::encode(haffNode *pn, std::string code){
    pn->code = code;
    if (pn->left)
        encode(pn->left, code + "0");
    if (pn->right)
        encode(pn->right, code + "1");
    if (!pn->left && !pn->right){
        codeMap[pn->uchar] = code;
    }
}

// 压缩函数
bool Compresser::compress(std::string sourcefile, std::string targetfile){
    codeMap.clear();
    std::ifstream inFile;
    std::ofstream outFile;
    if (sourcefile.substr(sourcefile.find_last_of(".") + 1) != "bo")
        return false; // 若源文件扩展名不是bo
    inFile.open(sourcefile, std::ios::in | std::ios::binary);
    if (!inFile)
        return false; // 打开源文件失败
    std::string newFileName = targetfile + ".co";
    outFile.open(newFileName, std::ios::out | std::ios::binary);
    if (!outFile){
        inFile.close();
        return false; // 打开目标文件失败
    }

    // 统计词频
    unsigned char uchar;
    std::map<unsigned char, unsigned long long> freqMap;
    while (inFile.read((char *)&uchar, sizeof(char))){
        freqMap[uchar]++;
    }

    // 建立词频小顶堆，升序队列
    std::priority_queue<haffNode *, std::vector<haffNode *>, Compare> freqHeap;
    std::map<unsigned char, unsigned long long>::reverse_iterator it;
    for (it = freqMap.rbegin(); it != freqMap.rend(); it++){
        haffNode *pn = new (haffNode);
        pn->freq = it->second;
        pn->uchar = it->first;
        pn->left = pn->right = 0;
        freqHeap.push(pn);
    }

    // 循环构建哈夫曼树
    while (freqHeap.size() > 1){
        haffNode *pn1 = freqHeap.top();
        freqHeap.pop();
        haffNode *pn2 = freqHeap.top();
        freqHeap.pop();
        haffNode *pn = new (haffNode);
        pn->freq = pn1->freq + pn2->freq;
        pn->left = pn1;
        pn->right = pn2;
        freqHeap.push(pn);
    }
    haffNode *root = freqHeap.top();
    codeMap.clear();

    // 利用生成的哈夫曼树对叶节点编码
    encode(root, "");
    // 补零数（暂时留空）写入压缩文件头部
    const unsigned char zeroUC = 0;
    outFile.write((char *)&zeroUC, sizeof(zeroUC));

    // 频率表写入压缩文件头部
    std::string freqTable;
    const unsigned long long zeroULL = 0;
    int i = 0;
    while (i < 256){
        if (freqMap.count(i) == 0){
            outFile.write((char *)&zeroULL, sizeof(zeroULL));
        }else{
            unsigned long long freq = freqMap[i];
            outFile.write((char *)&freq, sizeof(freq));
        }
        i++;
    }

    // 写入压缩文件主体（加密），最后补上补零数
    {
        inFile.clear();
        inFile.seekg(0);
        std::string buf;
        unsigned char uchar;
        while (inFile.read((char *)&uchar, sizeof(uchar))){
            buf += codeMap[uchar];
            while (buf.length() >= 8){
                std::bitset<8> bs(buf.substr(0, 8));
                uchar = bs.to_ulong();
                outFile.write((char *)&uchar, sizeof(uchar));
                buf = buf.substr(8);
            }
        }
        // 末尾处理
        int zeroNum = (8 - buf.length()) % 8;
        if (zeroNum){
            for (int i = 0; i < zeroNum; i++){
                buf += "0";
            }
            std::bitset<8> bs(buf.substr(0, 8));
            uchar = bs.to_ulong();
            outFile.write((char *)&uchar, sizeof(uchar));
        }
        // 写入头部预留的补零数
        outFile.clear();
        outFile.seekp(0);
        std::cout<< std::endl << "补零数:" << zeroNum << std::endl;
        uchar = zeroNum;
        outFile.write((char *)&uchar, sizeof(uchar));
    }
    inFile.close();
    outFile.close();
    // 正常执行
    return true;
}

// 解压部分
void Compresser::creat_tree(haffNode *father, unsigned char uchar, std::string code){
    if (code.empty()){
        father->uchar = uchar;
        return;
    }
    char way = code[0];
    if (way == '0'){
        if (!(father->left)){
            haffNode *son = new (haffNode);
            father->left = son;
        }
        creat_tree(father->left, uchar, code.substr(1));
    }
    else{
        if (!(father->right)){
            haffNode *son = new (haffNode);
            father->right = son;
        }
        creat_tree(father->right, uchar, code.substr(1));
    }
}
// 解压函数，sourcepath为压缩后源文件，destinationpath为存放压缩后文件的目录
bool Compresser::decompress(std::string sourcePath, std::string destinationDir){
    // 打开源文件
    if (sourcePath.substr(sourcePath.find_last_of(".") + 1) != "co"){
        std::cout << "源文件格式不正确\n";
        return false; // 源文件扩展名不是co
    }
    std::ifstream inFile;
    inFile.open(sourcePath, std::ios::in | std::ios::binary);
    if (!inFile){
        std::cout << "源文件打开失败\n";
        return false; // 打开源文件失败
    }
    unsigned char uchar;
    inFile.read((char *)&uchar, sizeof(char));
    int zeroNum = uchar;

    std::string newFileName;
    char sourcefilepath[PATH_MAX];
    strcpy(sourcefilepath, sourcePath.substr(0, sourcePath.find_last_of(".")).c_str());
    newFileName = destinationDir + '/' + basename(sourcefilepath);

    // 打开目标文件
    std::ofstream outFile;
    std::cout << newFileName << std::endl;
    outFile.open(newFileName, std::ios::out | std::ios::binary);
    if (!outFile){
        inFile.close();
        std::cout << "目标文件打开失败\n";
        return false; // 打开目标文件失败
    }
    // 读出频率表
    unsigned long long freq;
    std::map<unsigned char, unsigned long long> freqMap;
    int i = 0;
    for (i = 0; i < 256; i++){
        inFile.read((char *)&freq, sizeof(freq));
        if (freq){
            freqMap[i] = freq;
        }
    }

    if (i != 256){
        std::cout << "文件过短\n";
        return false; // 文件过短，频率表不完整
    }

    // 建立词频小顶堆
    std::priority_queue<haffNode *, std::vector<haffNode *>, Compare> freqHeap;
    std::map<unsigned char, unsigned long long>::reverse_iterator it;
    for (it = freqMap.rbegin(); it != freqMap.rend(); it++){
        haffNode *pn = new (haffNode);
        pn->freq = it->second;
        pn->uchar = it->first;
        pn->left = pn->right = 0;
        freqHeap.push(pn);
    }
    /**构建哈夫曼树**/
    while (freqHeap.size() > 1){
        haffNode *pn1 = freqHeap.top();
        freqHeap.pop();
        haffNode *pn2 = freqHeap.top();
        freqHeap.pop();
        haffNode *pn = new (haffNode);
        pn->freq = pn1->freq + pn2->freq;
        pn->left = pn1;
        pn->right = pn2;
        freqHeap.push(pn);
    }
    haffNode *root = freqHeap.top();
    codeMap.clear();

    // 读出主体，用哈夫曼树树解码
    haffNode *decodePointer = root;
    std::string buf, now;
    inFile.read((char *)&uchar, sizeof(unsigned char));

    std::bitset<8> bs = uchar;
    buf = bs.to_string();
    while (inFile.read((char *)&uchar, sizeof(unsigned char))){

        std::bitset<8> bs = uchar;
        now = buf;
        buf = bs.to_string();
        for (char i = 0; i < 8; i++){
            if (now[i] == '0') {
                if (!decodePointer->left){
                    std::cout << "解码错误\n";
                    return false; // 解码错误
                }
                decodePointer = decodePointer->left;
            }else{
                if (!decodePointer->right){
                    std::cout << "解码错误\n";
                    return false; // 解码错误
                }
                decodePointer = decodePointer->right;
            }

            if (!(decodePointer->left || decodePointer->right)){
                // std::cout<<int(decodePointer->uchar)<<" ";
                outFile.write((char *)&(decodePointer->uchar), sizeof(decodePointer->uchar));
                decodePointer = root;
            }
        }
    }

    // 最后一字节
    now = buf;
    for (char i = 0; i < (8 - zeroNum) % 8; i++){
        if (now[i] == '0'){
            if (!decodePointer->left){
                std::cout << "解码错误\n";
                return false; // 解码错误
            }
            decodePointer = decodePointer->left;
        }else{
            if (!decodePointer->right){
                std::cout << "解码错误\n";
                return false; // 解码错误
            }
            decodePointer = decodePointer->right;
        }
        if (!(decodePointer->left || decodePointer->right)){
            // std::cout<<int(decodePointer->uchar)<<" ";
            outFile.write((char *)&(decodePointer->uchar), sizeof(unsigned char));
            decodePointer = root;
        }
    }
    inFile.close();
    outFile.close();
    if (decodePointer == root)
        return true; // 正常执行
    else{
        std::cout << "结尾不完整\n";
        return false; // 文件结尾不完整
    }
}
