# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

校园导航系统 — 基于 C++17 的命令行批处理程序，接收标准输入的命令，输出到标准输出。助教通过 `./CampusNavigation < command.txt > answer.txt` 评测。

## 构建与测试

```bash
# 在项目根目录配置并构建
cmake -S CampusNavigation -B build -G "Ninja"
cmake --build build --config Debug

# 批处理运行（主程序）
./build/CampusNavigation < 测试数据_v2/必做/small_cases/case_01/command.txt > my_output.txt

# 单元测试（需在 CampusNavigation/ 目录下运行，因为测试程序读取同目录的 csv 文件）
cd CampusNavigation && ../build/test_lgraph.exe

# 与标准答案对比
diff my_output.txt 测试数据_v2/必做/small_cases/case_01/answer.txt
```

## 架构

5 个模块，分层调用：

```
main.cpp → CommandProcessor → CsvIO / Algorithm / LGraph
```

- **LGraph** — 图数据结构。嵌套 `unordered_map<string, unordered_map<string, EdgeNode>>` 存储，无向图双向维护。提供顶点/边的增删改查、道路开关、遍历查询。操作均抛 `GraphException`。
- **CsvIO** — CSV 读写。`ReadPlaces`/`ReadRoads`（跳过表头+空行，逗号换空格解析），`WritePlaces`/`WriteRoads`（字典序输出，带表头）。
- **CommandProcessor** — 解析命令行，分发到各 `cmd*` 方法，输出规范格式结果。`ProcessCommand` 返回 false 表示 QUIT。
- **Algorithm** — 图算法：DSU、连通分量、Dijkstra 最短路、MST(Kruskal)、关键节点/边。
- **main.cpp** — 循环 `getline(cin)`，调 `ProcessCommand`，QUIT 退出。

## 实现进度（对照 plan.md 阶段）

| 阶段 | 状态 |
|------|------|
| 1. LGraph 数据结构 | 已完成 |
| 2. CsvIO CSV 读写 | 已完成 |
| 3. CommandProcessor 命令处理 | 待实现 |
| 4. Algorithm 图算法 | 待实现 |
| 5. CommandProcessor 算法命令 | 待实现 |
| 6. 拓展1 分层图 SHORTEST_K | 待实现 |
| 7. 拓展2 自定义数据集 | 待实现 |
| 8. 拓展3 图形化界面 | 待实现 |

## 项目工作流规则

### 1. 每步写 commit 描述并确认
- 完成每个子任务（如 1.2、1.3 等）后，须给出该步骤对应的 git commit 描述（中文，不实际执行 commit，仅给出描述文本）。
- 每完成一个子步骤，必须让用户确认后才能继续下一步。
- **Why:** 用户希望每一步有清晰的版本记录，方便回溯和 review；同时确保每一步实现方向正确。

### 2. 会话结束自动导出对话记录
- 用户说"结束这次的工作"或类似含义时，将本次对话关键内容整理为 markdown 文件保存到 `对话记录/` 文件夹。
- 文件名格式：`对话记录_YYYY-MM-DD.md`（同一天多次则加 `_2`、`_3` 后缀）。
- **Why:** 用户可以保存对话历史做记录，并在后续会话中继续未完成的工作。
