#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <chrono> 
#include <algorithm>
using namespace std;   
const string CONFIG_DIR_PATH = "/data/media/0/Android/Manjusaka/自适应分辨率";
const string CONFIG_FILE_PATH = CONFIG_DIR_PATH + "/Manjusaka_Resolution.conf";
const string LOG_FILE_PATH = CONFIG_DIR_PATH + "/Manjusaka_Resolution.log";
const string DEFAULT_RESOLUTION_CMD = "wm size | grep Physical | cut -d ':' -f2 | xargs echo";
const string CURRENT_APP_NAME_CMD = "dumpsys activity | grep mResume | cut -d '/' -f1 | awk -F' ' '{print $4}'";
const string CURRENT_RESOLUTION_CMD_1 = "wm size | grep Physical | cut -d ':' -f2 | xargs echo";
const string CURRENT_RESOLUTION_CMD_2 = "wm size | grep Override | cut -d ':' -f2 | xargs echo";
ofstream log_f1(LOG_FILE_PATH.c_str());

// 获取默认分辨率
string get_default_resolution()
{
    auto start_time = chrono::high_resolution_clock::now(); // 开始计时
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

    FILE *fp = popen(DEFAULT_RESOLUTION_CMD.c_str(), "r");
    if (fp == NULL)
    {
        log_f1 << "[" << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S") << "]" << "无法获取默认分辨率。" << endl;
        exit(1);
    }
    char buf[128] = {0};
    fgets(buf, sizeof(buf), fp);
    pclose(fp);

    string default_res = string(buf);
    default_res.erase(default_res.find_last_not_of("\n") + 1);

    auto end_time = chrono::high_resolution_clock::now(); // 结束计时
    auto time_diff = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    return default_res;
}

// 获取当前应用程序名称
string get_current_app_name()
{
    auto start_time = chrono::high_resolution_clock::now(); // 开始计时
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    FILE *fp = popen(CURRENT_APP_NAME_CMD.c_str(), "r");
    if (fp == NULL)
    {
        log_f1 << "[" << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S") << "]" << "无法获取当前应用程序名称。" << endl;
        exit(1);
    }
    char buf[128] = {0};
    fgets(buf, sizeof(buf), fp);
    pclose(fp);

    string app_name = string(buf);
    app_name.erase(app_name.find_last_not_of("\n") + 1);

    auto end_time = chrono::high_resolution_clock::now(); // 结束计时
    auto time_diff = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    return app_name;
}

// 获取当前分辨率
string get_current_resolution()
{
    auto start_time = chrono::high_resolution_clock::now(); // 开始计时
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

    string current_res;
    {
        ifstream ifs(CURRENT_RESOLUTION_CMD_1.c_str());
        getline(ifs, current_res);
        ifs.close();
        current_res.erase(current_res.find_last_not_of("\n") + 1);
    }

    if (current_res.empty())
    {
        {
            ifstream ifs(CURRENT_RESOLUTION_CMD_2.c_str());
            getline(ifs, current_res);
            ifs.close();
            current_res.erase(current_res.find_last_not_of("\n") + 1);
        }
    }

    auto end_time = chrono::high_resolution_clock::now(); // 结束计时
    auto time_diff = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    return current_res;
}

int main()
{
    // 创建配置文件夹及日志文件
    if (mkdir(CONFIG_DIR_PATH.c_str(), 0755) != 0)
    {
        if (access(CONFIG_DIR_PATH.c_str(), F_OK) != 0)
        {
            exit(1);
        }
    }
    ofstream log_file(LOG_FILE_PATH.c_str());
    chmod(LOG_FILE_PATH.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    string default_res = get_default_resolution();
    string prev_res;
    bool is_restored_default_res = false;
    bool kunkun = false;
    // 如果配置文件不存在，则创建一个示例配置文件
    if (!ifstream(CONFIG_FILE_PATH))
    {
        ofstream config_file(CONFIG_FILE_PATH.c_str());
        config_file << "# Author:Manjusaka(酷安@曼珠沙华Y)" << std::endl;
        config_file << "# Group:647299031" << std::endl;
        config_file << "# QQ:898780441" << std::endl;
        config_file << "# 格式为 包名 分辨率" << endl;
        config_file << "# 以下为示例" << endl;
        config_file << "com.tencent.tmgp.sgame 1080x2200" << endl;
        config_file << "com.termux 1060x2460" << endl;
        config_file.close();
        chmod(CONFIG_FILE_PATH.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    }

    while (true)
    {
        auto loop_start_time = chrono::high_resolution_clock::now(); // 记录每次循环开始的时间

        time_t cur_time = time(NULL);
        tm *timeinfo = localtime(&cur_time);

        string current_app_name = get_current_app_name();
        string current_res = get_current_resolution();

        // 读取配置文件，查找是否存在当前应用程序的包名
        bool found = false;
        string target_res;
        {
            ifstream config_file(CONFIG_FILE_PATH.c_str());
            string line;
            while (getline(config_file, line))
            {
                size_t pos = line.find(" ");
                if (pos == string::npos || line[0] == '#')
                    continue;
                string app_name = line.substr(0, pos);
                target_res = line.substr(pos + 1);

                if (app_name == current_app_name)
                {
                    found = true;
                    break;
                }
            }
            config_file.close();
        }

        if (found)
        {
        
        if (!kunkun) 
        {
            if (current_res != target_res)
            {   
                auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
                prev_res = current_res;
                string cmd = "wm size " + target_res;
                system(cmd.c_str());
                log_file << "[" << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S") << "]" << "分辨率已更改为: " << target_res << endl;
                is_restored_default_res = false;
                kunkun = true;
            }
        }    
            
        }
        else
        {
            if (!is_restored_default_res) // 如果没有恢复过默认分辨率
            {
                if (!prev_res.empty()) // 如果有修改过分辨率
                {
                    if (current_res != prev_res) // 如果当前分辨率与之前不同，才恢复上一个分辨率
                    {   
                        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

                        // 恢复上一个分辨率
                        string cmd = "wm size " + prev_res;
                        system(cmd.c_str());
                        log_file << "[" << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S") << "]" << "分辨率已恢复到: " << prev_res << endl;
                        is_restored_default_res = false;
                        kunkun = false;
                    }
                    // 清空 prev_res
                    prev_res.clear();
                }
                else if (current_res != default_res)
                {   
                    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

                    string cmd = "wm size " + default_res;
                    system(cmd.c_str());
                    log_file << "[" << put_time(localtime(&now), "%Y-%m-%d %H:%M:%S") << "]" << "分辨率已恢复到默认值: " << default_res << endl;
                    is_restored_default_res = true;
                    kunkun = false;
                }
            }
        }

        auto loop_end_time = chrono::high_resolution_clock::now(); // 记录每次循环结束的时间
        auto loop_time_diff = chrono::duration_cast<chrono::microseconds>(loop_end_time - loop_start_time);
     int sleep_time = std::max<long long>(1000000 - loop_time_diff.count(), 0);
        usleep(sleep_time); // 微秒级别的休眠
    }

    log_file.close();
    return 0;
}