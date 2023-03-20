#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

int main() {
    string configFile = "/data/media/0/Android/Manjusaka/自定义软连接/Manjusaka_Softlink.ini";
    string logFile = "/data/media/0/Android/Manjusaka/自定义软连接/Manjusaka_Softlink.log";
    string mainFolder = "/data/media/0/Android/data/com.tencent.mobileqq/tencent/QQfile_recv/";
    string baseFolder;
    string softLinkFolder = "/data/media/0/Android/Manjusaka/自定义软连接/Soft-link/";

    ifstream configFileReader(configFile);
    if (configFileReader.is_open()) {
        string line;
        while (getline(configFileReader, line)) {
            if (line.find("baseFolder=") != string::npos) {
                baseFolder = line.substr(line.find("=")+1);
                break;
            }
        }
        configFileReader.close();
    }

    if (baseFolder.empty()) {
        baseFolder = "/data/media/0/Android/Manjusaka/自定义软连接/Soft-link/";
    }

   if(access(softLinkFolder.c_str(), F_OK) != 0){
    int status = mkdir(softLinkFolder.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if(status == -1 && errno == ENOENT){ // 上级目录不存在，创建上级目录
        size_t pos = softLinkFolder.rfind("/", softLinkFolder.length()-2);
        string parentFolder = softLinkFolder.substr(0, pos);
        int ret = mkdir(parentFolder.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if(ret == 0){ // 创建上级目录成功，再次尝试创建软连接目录
            int status = mkdir(softLinkFolder.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if(status != 0){
                cerr << "无法创建文件夹：" << softLinkFolder << endl;
                return 1;
            }
        }else{ // 创建上级目录失败
            cerr << "无法创建文件夹：" << parentFolder << endl;
            return 1;
        }
    }else if(status != 0){ // 创建软连接目录失败
        cerr << "无法创建文件夹：" << softLinkFolder << endl;
        return 1;
    }
}

    while (true) {
        // 如果配置文件不存在，则创建一个示例
        ifstream configFileReader(configFile);
        if (!configFileReader.good()) {
            ofstream newFile(configFile.c_str());
            if (newFile.is_open()) {
                newFile << "# Author:Manjusaka(酷安@曼珠沙华Y)" << endl;
                newFile << "# Group:647299031" << endl;
                newFile << "# QQ:898780441" << endl;
                newFile << "# 示例配置文件" << endl;
                newFile << "# 软连接到的文件夹" << endl;
                newFile << "baseFolder=" << baseFolder << endl;
                newFile << "# 将以下目录软连接到 " << baseFolder << " 文件夹内" << endl;
                newFile << "[QQ下载]" << endl;
                newFile << mainFolder << endl;
                newFile.close();
            } else {
                cerr << "无法创建文件 " << configFile << endl;
                return 1;
            }
        }
        configFileReader.close();

        ifstream configFileReader2(configFile.c_str());
        ofstream logFileWriter(logFile.c_str(), ios::app);
        if (configFileReader2.is_open() && logFileWriter.is_open()) {
            string line, folderName, folderPath;
            while (getline(configFileReader2, line)) {
                // 忽略注释和空行以及baseFolder
                if (line[0] == '#' || line.empty() || line.find("baseFolder=") != string::npos)
                    continue;

                // 如果是目录名称，则设置当前目录名为该行内容
                if (line[0] == '[' && line[line.size()-1] == ']') {
                    folderName = line.substr(1, line.size()-2);
                } else {
                    // 否则，将当前行内容作为目录路径
                    folderPath = line;

                    // 判断是否为文件夹或者是其他类型的路径
                    struct stat s;
                    if (stat(folderPath.c_str(), &s) == 0) {
                        if (S_ISDIR(s.st_mode)) {
                            // 创建软链接并记录日志
                            string linkPath = baseFolder + folderName;
                            if (access(linkPath.c_str(), F_OK) == 0) { // 若已有软连接则输出到日志
                                logFileWriter << linkPath << " 已经有与之软连接到的地址" << endl;
                            } else {
                                int result = symlink(folderPath.c_str(), linkPath.c_str());
                                if (result == 0) {
                                    logFileWriter << folderPath << " 软链接成功：" << linkPath << endl;
                                } else {
                                    logFileWriter << folderPath << " 软链接失败：" << linkPath << endl;
                                }
                            }
                        } else {
                            logFileWriter << folderPath << " 不是文件夹，无法进行软链接" << endl;
                        }
                    } else {
                        logFileWriter << folderPath << " 路径不存在，无法进行软链接" << endl;
                    }

                }
            }
            configFileReader2.close();
            logFileWriter.close();
        } else {
            cerr << "无法打开文件 " << configFile << " 或 " << logFile << endl;
            return 1;
        }

        // 休眠 1 分钟
        sleep(60);
    }

    return 0;
}
