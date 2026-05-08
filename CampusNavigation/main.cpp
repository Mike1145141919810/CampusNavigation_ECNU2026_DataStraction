//
// 动态图上的校园路线规划与设施分析系统
// main.cpp - 命令驱动主程序
//
// 用法：
//   ./CampusNavigation                          从标准输入逐行读取命令
//   ./CampusNavigation < command.txt            从文件重定向读取命令
//   ./CampusNavigation < command.txt > answer.txt  批处理模式
//

#include <iostream>
#include <string>
#include "LGraph.h"
#include "CommandProcessor.h"

int main() {
    Graph::LGraph graph(false);  // 初始化一个无向图
    Graph::CommandProcessor processor(graph);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (!processor.ProcessCommand(line)) {
            break;  // 收到 QUIT 命令，退出
        }
    }

    return 0;
}
