# 校园导航系统 — 详细实现计划

## 项目概述

实现一个支持动态图更新、路径规划、连通分析和关键节点分析的校园路线规划系统。当前代码为骨架（全是 TODO），需要全部实现。完成后可选做 3 个拓展任务。

**总分结构**：必做 75 + 报告 10 + 拓展 15 = 100 分

**用户确认：**
- 全部三个拓展都做（拿满 15 分）
- 存储结构用嵌套 unordered_map

---

## 存储架构设计

### LGraph 内部存储

采用 **嵌套 unordered_map** 作为核心存储结构：

```cpp
std::unordered_map<std::string, LocationInfo> vertices_;  // place_id → 顶点信息
std::unordered_map<std::string,
    std::unordered_map<std::string, EdgeNode>> adj_;       // from → {to → 边}
bool directed_;
```

**理由：**
- 顶点/边增删查均为平均 O(1)
- 无向图双向存储，删除顶点时只需遍历邻接表清理反向边
- 无需维护索引映射，代码简洁
- 对 10000 节点规模完全够用

---

## 实施阶段

### 阶段 1：LGraph — 图数据结构实现

**涉及文件：** [LGraph.h](CampusNavigation/LGraph.h), [LGraph.cpp](CampusNavigation/LGraph.cpp)

| 任务 | 说明 |
|------|------|
| 1.1 实现私有存储 | 嵌套 unordered_map 结构 + directed 标志 |
| 1.2 基础信息 | `VertexCount()`, `EdgesCount()`, `exist_vertex()`, `exist_edge()` |
| 1.3 顶点操作 | `InsertVertex()`, `DeleteVertex()`（同步删除相关边）, `UpdateVertex()`, `GetVertex()` |
| 1.4 边操作 | `InsertEdge()`, `DeleteEdge()`, `UpdateEdge()`, `GetEdge()` — 注意无向图双向同步 |
| 1.5 道路状态 | `CloseRoad()`, `OpenRoad()` — 复用 UpdateEdge |
| 1.6 遍历/查询 | `AllPlaceIds()`, `AllEdges(only_open)`, `GetAdjacentEdges()`, `GetPlacesByCategory()` |

**注意：** AllEdges() 需避免无向图重复输出（只输出 u < v 或 u 字典序 < v 的方向）。

---

### 阶段 2：CsvIO — CSV 读写

**涉及文件：** [CsvIO.cpp](CampusNavigation/CsvIO.cpp)

| 任务 | 说明 |
|------|------|
| 2.1 ReadPlaces | 打开文件，逐行解析（逗号→空格），跳过表头和空行 |
| 2.2 ReadRoads | 同上，解析道路字段 |
| 2.3 WritePlaces | 写表头，遍历 AllPlaceIds() + GetVertex() 输出 |
| 2.4 WriteRoads | 写表头，遍历 AllEdges(false) 输出（含 closed 边） |

---

### 阶段 3：CommandProcessor — 数据维护与查询命令

**涉及文件：** [CommandProcessor.cpp](CampusNavigation/CommandProcessor.cpp)

| 命令 | 实现要点 |
|------|----------|
| LOAD | 调 CsvIO::ReadPlaces/ReadRoads，逐个 InsertVertex/InsertEdge，清空旧图 |
| SAVE | 调 CsvIO::WritePlaces/WriteRoads |
| ADD_PLACE | 参数校验，InsertVertex，捕获异常→ERROR |
| DELETE_PLACE | DeleteVertex，捕获异常 |
| UPDATE_PLACE | UpdateVertex，捕获异常 |
| ADD_ROAD | InsertEdge |
| DELETE_ROAD | DeleteEdge |
| UPDATE_ROAD | UpdateEdge |
| CLOSE_ROAD / OPEN_ROAD | CloseRoad / OpenRoad |
| QUERY_PLACE | GetVertex → 格式化输出 `PLACE <id> <name> <cat> <stay> <open> <close>` |
| QUERY_CATEGORY | GetPlacesByCategory → 字典序排序 → 输出 |
| ADJ | GetAdjacentEdges → 按 neighbor_id 字典序排序 → 格式化输出 |

**排序保证：** QUERY_CATEGORY 按字典序，ADJ 按 neighbor 字典序。

---

### 阶段 4：Algorithm — 图算法

**涉及文件：** [Algorithm.h](CampusNavigation/Algorithm.h), [Algorithm.cpp](CampusNavigation/Algorithm.cpp)

#### 4.1 DSU（并查集）
- 实现带路径压缩的 `find()` + 按秩合并的 `unite()`
- 用于 Kruskal MST 和算法中的连通性判断

#### 4.2 连通分量分析 `GetConnectedComponents()`
- DFS/BFS 遍历所有顶点，仅走 status=open 的边
- 返回分量个数和降序排列的规模列表

#### 4.3 最短路径 `GetShortestPath()` — Dijkstra
- 优先队列（小顶堆）优化
- 根据 PathMode 选择边权（DIST→distance，TIME→walk_time）
- 仅走 status=open 的边
- `unordered_map<string, int>` 维护 dist
- `unordered_map<string, string>` 维护 prev 用于路径回溯
- 不可达时 reachable=false

#### 4.4 时刻约束最短路径 `GetTimedShortestPath()`
- 先检查起点/终点在 time 时刻是否开放
- 在 Dijkstra 松弛时额外检查：邻居顶点在 time 是否开放
- 其余与普通 Dijkstra 相同

#### 4.5 必经点路径 `GetMustPassPath()`
- 路径拆段：from→w1, w1→w2, ..., wk→to
- 每段调 GetShortestPath，段间不重复中间点
- 任一段不可达则整体 NO_PATH

#### 4.6 最小生成树 `MinimumSpanningTree()` — Kruskal
- 收集所有 open 边，按 distance 升序排序
- DSU 维护连通性，贪心加入边
- 最终边数 ≠ 顶点数-1 → 不连通，返回空
- 输出边按 `(min(u,v), max(u,v))` 字典序排序

#### 4.7 关键节点/边 `FindCriticalNodesAndEdges()`
- 先算 baseline 连通分量数
- **关键节点**：对每个顶点，临时排除（不加入遍历），重算分量数 → 增加则是关键节点
- **关键边**：对每条 open 边，临时排除，重算分量数 → 增加则是关键边
- O(V·(V+E)) + O(E·(V+E))，千节点规模可接受

---

### 阶段 5：CommandProcessor — 算法命令

**涉及文件：** [CommandProcessor.cpp](CampusNavigation/CommandProcessor.cpp)

| 命令 | 算法函数 | 输出格式 |
|------|----------|----------|
| COMPONENTS | GetConnectedComponents | `COMPONENTS <count> SIZES <s1> <s2> ...` |
| SHORTEST | GetShortestPath | `PATH <DIST\|TIME> <total> NODES <id1> ...` |
| TIMED_SHORTEST | GetTimedShortestPath | 同上 |
| MUST_PASS | GetMustPassPath | 同上 |
| MST | MinimumSpanningTree | `MST <total> EDGES <u>-<v>:<w> ...` |
| CRITICAL | FindCriticalNodesAndEdges | `CRITICAL NODES <n> <id1> ... EDGES <e> <u1>-<v1> ...` |
| QUIT | — | 无输出，返回 false |

所有算法命令需先验证顶点存在性，不存在输出 `ERROR place_not_found`。

---

### 阶段 6：拓展 1 — 分层图最短路径 SHORTEST_K

**涉及文件：** [Algorithm.h](CampusNavigation/Algorithm.h), [Algorithm.cpp](CampusNavigation/Algorithm.cpp), [CommandProcessor.cpp](CampusNavigation/CommandProcessor.cpp)

#### 算法：分层图 DP（K 层状态图 Dijkstra）

**状态定义：** `dist[place_id][k]` = 到达 place_id 恰好用 k 张券的最短时间

**状态转移（对边 u→v, walk_time = w）：**
1. 不用券：`dist[v][k] = min(dist[v][k], dist[u][k] + w)`
2. 用券（k+1 ≤ K）：`dist[v][k+1] = min(dist[v][k+1], dist[u][k] + ceil(w/3))`

**实现：**
- 优先队列存 `(time, place_id, k_used)`
- `prev[v][k]` 回溯路径
- `used_coupon[v][k]` 标记到该状态的边是否用了券
- 终止：从优先队列取出 `(to_id, k)` 任意 k ≤ K，取最小 time

**路径回退**：从目标状态 `(to_id, best_k)` 回溯到起始 `(from_id, 0)`，记录用券的边。

**关键反例（卡贪心）：**
```
A-B-C-D  walk = (10, 10, 10)
A-X-D    walk = (5, 50)
```
K=1 时正确答案 22（走 A-X-D，在 X-D 用券），而非对原最短路 ABCD 最长边用券的 24。

#### CommandProcessor 新增
- `cmdShortestK()` — 解析 `SHORTEST_K <from> <to> <K>`
- 输出格式：`PATH <total_time> K_USED <k_used> NODES <id1> ... FAST <count> [<u>-<v> ...]`

---

### 阶段 7：拓展 2 — 自定义微型数据集 + 对抗样例

**涉及目录：** `自定义数据集/`

#### 微型数据集（12~20 节点，20~40 边）
- 可能场景：校园局部区域 / 地铁网络 / 景区
- 附说明文档说明数据来源、建模特点和测试目标

#### 对抗样例
- 针对**贪心分层图**的反例（可复用测试数据的 case_layered_01 思路）
- 针对**必经点顺序不敏感**的错误实现
- 针对**不考虑时间窗口**的 timed_shortest 错误实现

---

### 阶段 8：拓展 3 — 图形化界面（可选）

**技术选型建议：** HTML + Canvas（无需额外库，浏览器即可打开）

- 显示节点位置（手动布局坐标存别处或自动力导向布局）
- 绘制边连接（open/closed 不同颜色）
- 最短路径高亮（用户输入起终点后调用 CLI 取得结果再渲染）
- 关键节点/边标识

需用截图和简短技术说明提交。

---

## 构建与测试

### 构建命令（Windows + MinGW + Ninja）
```bash
cd CampusNavigation
cmake -S . -B build -G "Ninja"
cmake --build build --config Debug
```

### 测试方法
```bash
# 运行单个测试 case
./CampusNavigation/build/CampusNavigation < "../测试数据_v2/必做/small_cases/case_01/command.txt"

# 与标准答案对比
./CampusNavigation/build/CampusNavigation < "../测试数据_v2/必做/small_cases/case_01/command.txt" > my_output.txt
diff my_output.txt "../测试数据_v2/必做/small_cases/case_01/answer.txt"
```

### 测试覆盖
| 测试数据 | 主要验证点 |
|----------|-----------|
| case_01~04 | 数据维护、查询、SHORTEST（双模式）、TIMED_SHORTEST、MST、CRITICAL |
| medium/* | 各拓扑形态下的算法正确性 + 性能 |
| large/chain_1000 | 1000 节点链上的 CRITICAL（998 关键节点）、性能 |
| large/random_sparse_10000 | 10000 节点压力测试 |
| 拓展_分层图/* | SHORTEST_K 正确性与性能 |

### 逐阶段验证策略
- **阶段 1-3 完成后：** case_01 中的 LOAD + 查询命令 + 增删改
- **阶段 4-5 完成后：** 所有必做 small_cases + medium_cases
- **阶段 4-5 完成后（性能）：** large_cases
- **阶段 6 完成后：** 拓展_分层图所有 case

---

## 关键设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 存储结构 | 嵌套 unordered_map | O(1) 增删查，实现简单，适合动态图 |
| 无向图策略 | 双向存储 | 查询快，删除需双向同步 |
| Dijkstra | 优先队列 + unordered_map | 经典实现，10000 节点秒级 |
| MST | Kruskal + DSU | 实现简单，不受图形态影响 |
| 关键节点/边 | 暴力 BFS 重算 | 实现简单直接，符合课程要求；Tarjan 作为加分 |
| 分层图 | 状态图 Dijkstra (V·K) | K≤10 时极高效，保证最优 |
