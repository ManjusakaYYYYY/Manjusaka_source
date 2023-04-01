#include <iostream>
#include <cstdlib>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
    cout << "- Author:Manjusaka(酷安@曼珠沙华Y)" << endl;
    cout << "- Group:647299031" << endl;
    cout << "- QQ:898780441" << endl;
    if (system("[ -d \"Manjusaka_Done\" ] || mkdir -p \"Manjusaka_Done\" 2> /dev/null") != 0) {
        // 如果执行失败，则返回错误码
        return 1;
    }

    DIR* dir;
    struct dirent* ptr;
    if ((dir=opendir(".")) == NULL) {
        // 如果打开目录失败，则返回错误码
        return 1;
    }

    while ((ptr=readdir(dir)) != NULL) {
        string file_name = ptr->d_name;
        if (file_name == "ManjusakaDecryption" || ptr->d_type == DT_DIR) {
            continue;
        }
        string cmd = "./" + file_name;
        if (access(cmd.c_str(), X_OK) != 0) {
            continue;
        }
        cout << "- 正在解密: " << file_name << endl;
        pid_t pid;
        if ((pid = fork()) < 0) {
            // 如果 fork 进程失败，则返回错误码
            return 1;
        } else if (pid == 0) {
            // 如果是子进程则执行指定的命令
            execl(cmd.c_str(), cmd.c_str(), NULL);
            exit(1);
        } else {
            usleep(1000);
            string output = "Manjusaka_Done/" + file_name.substr(0, file_name.length()-3) + ".conf";
            string cmdline_file = "/proc/" + to_string(pid) + "/cmdline";
           if (system(("cp " + cmdline_file + " " + output + " 2> /dev/null").c_str()) != 0) {     // 如果执行拷贝命令失败，则返回错误码
                return 1;
            }
            if (kill(pid, SIGS
