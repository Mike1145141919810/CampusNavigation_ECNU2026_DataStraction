# 校园导航系统

基于 C++17 的命令行校园路线规划与设施分析系统，支持动态图更新、路径规划、连通分析和关键节点分析。

## 编译

```bash
# 在项目根目录配置并构建
cmake -S CampusNavigation -B build -G "Ninja"
cmake --build build --config Debug
```

**要求**：C++17（MinGW g++ 8.1.0+ 或同等编译器）、CMake 3.15+、Ninja。

## 运行

```bash
# 批处理模式（评测接口）
./build/CampusNavigation < command.txt > answer.txt

# 示例：运行 case_01
./build/CampusNavigation < 测试数据_v2/必做/small_cases/case_01/command.txt > my_output.txt
diff my_output.txt 测试数据_v2/必做/small_cases/case_01/answer.txt
```

## 命令列表

### 数据维护
| 命令 | 说明 |
|------|------|
| `LOAD <places.csv> <roads.csv>` | 从 CSV 加载图数据 |
| `SAVE <places.csv> <roads.csv>` | 保存图数据到 CSV |
| `ADD_PLACE <id> <name> <cat> <stay> <open> <close>` | 新增地点 |
| `DELETE_PLACE <id>` | 删除地点 |
| `UPDATE_PLACE <id> <field> <value>` | 修改地点字段 |
| `ADD_ROAD <from> <to> <dist> <time> <status>` | 新增道路 |
| `DELETE_ROAD <from> <to>` | 删除道路 |
| `UPDATE_ROAD <from> <to> <field> <value>` | 修改道路字段 |
| `CLOSE_ROAD <from> <to>` | 关闭道路 |
| `OPEN_ROAD <from> <to>` | 开放道路 |

### 查询
| 命令 | 说明 |
|------|------|
| `QUERY_PLACE <id>` | 查询地点信息 |
| `QUERY_CATEGORY <cat>` | 按类别查询地点 |
| `ADJ <id>` | 查询邻接道路 |

### 图算法
| 命令 | 说明 |
|------|------|
| `COMPONENTS` | 连通分量分析 |
| `SHORTEST <from> <to> <DIST\|TIME>` | 最短路径 |
| `TIMED_SHORTEST <from> <to> <HH:MM> <DIST\|TIME>` | 时刻约束最短路径 |
| `MUST_PASS <from> <to> <DIST\|TIME> <k> <p1>...<pk>` | 必经点路径 |
| `MST` | 最小生成树 |
| `CRITICAL` | 关键节点与关键边 |
| `SHORTEST_K <from> <to> <K>` | 分层图最短路径（拓展） |
| `QUIT` | 退出 |

## 文件结构

```
CampusNavigation/
├── main.cpp                  # CLI 入口
├── LGraph.h/cpp              # 图 ADT（嵌套 unordered_map）
├── LocationInfo.h            # 地点信息结构体
├── GraphException.h          # 异常类
├── CsvIO.h/cpp               # CSV 读写
├── Algorithm.h/cpp           # 图算法（DSU/Dijkstra/MST/Critical/Tarjan/SHORTEST_K）
├── CommandProcessor.h/cpp    # 命令解析与分发
├── graph_ui.html             # 图形化界面（拓展3）
├── 图形化界面技术说明.md
├── CMakeLists.txt
└── README.md
```

## 拓展功能

- **拓展 1**：分层图最短路径 `SHORTEST_K`（加速券）
- **拓展 2**：自定义微数据集 + 对抗样例（`../自定义数据集/`）
- **拓展 3**：HTML+Canvas 图形化界面（`graph_ui.html`，浏览器直接打开）
- **加分项**：Tarjan 算法 O(V+E) 求割点与桥（`FindCriticalTarjan`）
