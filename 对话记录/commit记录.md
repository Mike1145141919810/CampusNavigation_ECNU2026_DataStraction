# Commit 记录

## 阶段3：CommandProcessor 数据维护与查询命令

```
实现 CommandProcessor 阶段3：数据维护与查询命令

- 实现 13 个命令：LOAD/SAVE/QUERY_PLACE/QUERY_CATEGORY/ADJ/ADD_PLACE/DELETE_PLACE/UPDATE_PLACE/ADD_ROAD/DELETE_ROAD/UPDATE_ROAD/CLOSE_ROAD/OPEN_ROAD
- LGraph 新增 Clear() 方法支持 LOAD 清空旧图
- 错误前置检查（exist_vertex/exist_edge）输出标准化 ERROR 信息
- UPDATE_PLACE/UPDATE_ROAD 字段白名单校验，stay_time/distance/walk_time 整数校验
- QUERY_CATEGORY/ADJ 输出按字典序排序
- 算法命令（COMPONENTS/SHORTEST 等）留桩待阶段5
- case_01 前 20 行非算法命令与标准答案完全一致

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 阶段4：Algorithm 图算法实现

```
实现 Algorithm 阶段4：DSU 并查集、连通分量、Dijkstra、MST、关键节点/边

- 4.1 DSU：路径压缩 find + 按秩合并 unite，平摊 O(α(n))
- 4.2 GetConnectedComponents：BFS 仅走 open 边，降序输出规模；内部 bfsComponent 支持 blocked_vertex/blocked_edge 参数，供关键节点/边复用
- 4.3 GetShortestPath：优先队列 Dijkstra，DIST/TIME 双模式，unordered_map 存储 dist/prev
- 4.4 GetTimedShortestPath：在 Dijkstra 松弛时额外检查 open_time <= time <= close_time
- 4.5 GetMustPassPath：分段调用 GetShortestPath 后拼接去重
- 4.6 MinimumSpanningTree：Kruskal + DSU，按 distance 升序贪心加边
- 4.7 FindCriticalNodesAndEdges：暴力连通分量重算，O(V·(V+E)+E·(V+E))，复用 bfsComponent 内部函数
- PathResult 新增三参数构造函数

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 阶段5：CommandProcessor 算法命令接入

```
实现 CommandProcessor 阶段5：接入算法命令

- cmdComponents：调用 GetConnectedComponents，输出 COMPONENTS <count> SIZES ...
- cmdShortest：解析 DIST/TIME 模式，验证顶点存在，输出 PATH/NODES 或 NO_PATH
- cmdTimedShortest：解析时刻参数，调用 GetTimedShortestPath
- cmdMustPass：解析必经点列表，调用 GetMustPassPath
- cmdMst：调用 MinimumSpanningTree，输出 MST <total> EDGES ... 或 DISCONNECTED
- cmdCritical：调用 FindCriticalNodesAndEdges，输出 CRITICAL NODES/EDGES
- 修复 PathMode 命名空间限定（Algorithm::PathMode/Algorithm::TIME/Algorithm::DIST）
- case_01 全部 26 行与标准答案完全一致

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 阶段6：拓展1 — 分层图最短路径 SHORTEST_K

```
实现拓展1 阶段6：分层图最短路径 SHORTEST_K

- Algorithm.h 新增 PathResultK 结构体（total_time/k_used/path/fast_edges/reachable）
- Algorithm.cpp 实现 GetShortestPathK：状态扩展为 (place_id, k_used)，每边两分支转移：
  1. 不用券 dist[v][k] = min(dist[v][k], dist[u][k] + w)
  2. 用券 dist[v][k+1] = min(dist[v][k+1], dist[u][k] + ceil(w/3))
- 优先队列 tuple<time, place_id, k>，第一次弹出终点即最优
- 路径回溯记录 prev_node/prev_k/coupon，用券边按 canonical 字典序输出
- CommandProcessor 新增 cmdShortestK，FAST 边输出前排序修复 large 测试差异
- 4 组分层图测试全部通过（small/medium×2/large）

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 阶段7：拓展2 — 自定义微型数据集 + 对抗样例

```
实现拓展2 阶段7：自定义微型数据集 + 对抗样例

微数据集（华师大普陀校区）：
- 12 节点（地铁站、商圈、校园地标），40 条边（38 步行 + 2 地铁）
- 数据来源：百度地图、上海地铁官网、Moovit 公交查询
- 双层交通网络（步行+地铁），DIST/TIME 双模式可区分
- 差异化时间窗口（商场/公园/地铁/全天），支持 TIMED_SHORTEST 测试
- P0002 为关键节点（南北交通咽喉），附说明文档

对抗样例：
- 8 节点 20 边校园图，针对 TIMED_SHORTEST 中间节点时间窗检查
- 构造"短但有约束"vs"绕路无约束"对比路径
- 22:00 查询食堂（21:00关闭）→ 正确走绕路 12min，错误实现仅查起止点返回 4min
- 附说明文档详述对抗目标、卡住的错误、正确做法、实测验证结果

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 阶段8：拓展3 — HTML+Canvas 图形化界面

```
实现拓展3 阶段8：HTML+Canvas 图形化界面

- 单文件自包含（graph_ui.html），零依赖，浏览器直接打开
- 默认嵌入项目完整数据（87 节点 26 边），支持文件选择器加载任意 CSV
- 混合布局引擎：力导向（主连通分量）+ 同心圆环（非连通分量）
- 节点按 6 种分类着色，标签截断，悬停 tooltip，选中/路径红色光晕
- 边 open/closed 区分（实线绿色/虚线红色），关键边橙色加粗，路径边红色发光
- 最短路径高亮：DIST/TIME/SHORTEST_K 三种模式，Dijkstra/分层图 Dijkstra 前端实现
- 关键节点（黄色虚线环）+ 关键边（橙色粗线）暴力连通分量重算
- 连通分量凸包半透明着色 + 同心圆虚线引导圈
- 视图交互：滚轮缩放（0.12x~6x）、拖拽平移、双击放大、点击选点
- HiDPI 自适应、深色主题、附技术说明文档
- CLI 入口保留不受影响

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 修复：MST 等权边确定性 + Dijkstra 路径平局

```
修复 Algorithm：Kruskal MST 确定性边收集 + Dijkstra 等权路径平局

MST 修复：
- 改为从排序后的 AllPlaceIds 确定性遍历收集边（替代 unordered_map 随机顺序）
- Kruskal 等权边按 canonical key (min|max) 字典序平局
- 尝试了 Prim 算法（后回退为 Kruskal，Prim 在 10000 节点等权图上与 Kruskal 等价）

Dijkstra 修复：
- 等权路径（nd == dist[v]）时选字典序更小的前驱（u < prev[v]）
- GetShortestPath 和 GetTimedShortestPath 均已修复

测试结果：
- 必做 12/13 通过，random_sparse_10000 MST 不匹配为非代码 bug
  （双方总权 41994，9999 边中 9953 一致，46 条不同，均为有效 MST）
- 分层图 4/4 全部通过

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```

## 文档更新

```
更新文档：数据结构设计说明、图形化界面技术说明

- 数据结构设计说明.md：新增 Clear() 方法复杂度分析
- 图形化界面技术说明.md：更新功能描述（SHORTEST_K 模式、混合布局引擎、视图交互）

Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>
```
