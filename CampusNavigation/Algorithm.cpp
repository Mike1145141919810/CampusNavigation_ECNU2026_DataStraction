//
// 动态图上的校园路线规划与设施分析系统
// Algorithm.cpp - 图算法实现
//

#include "Algorithm.h"
#include <queue>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace Graph
{
    namespace Algorithm
    {

        // ==================== 内部辅助：带屏蔽的 BFS ====================

        // 从 start 出发 BFS，仅走 open 边。
        // blocked_vertex: 被"临时删除"的顶点（跳过，不访问不从它出发）
        // blocked_edge_keys: 被"临时关闭"的边的 canonical key 集合
        // visited: 会被修改，标记本次 BFS 访问到的顶点
        static void bfsComponent(const LGraph &graph,
                                 const std::string &start,
                                 std::unordered_set<std::string> &visited,
                                 const std::string &blocked_vertex,
                                 const std::unordered_set<std::string> &blocked_edge_keys)
        {
            std::queue<std::string> q;
            q.push(start);
            visited.insert(start);

            while (!q.empty())
            {
                std::string u = q.front();
                q.pop();

                auto edges = graph.GetAdjacentEdges(u);
                for (const auto &e : edges)
                {
                    if (e.status != "open")
                        continue;
                    const std::string &v = e.to_id;
                    if (v == blocked_vertex)
                        continue;
                    if (visited.count(v))
                        continue;

                    // 检查边是否被屏蔽
                    std::string ek = (u < v) ? (u + "|" + v) : (v + "|" + u);
                    if (blocked_edge_keys.count(ek))
                        continue;

                    visited.insert(v);
                    q.push(v);
                }
            }
        }

        // 计算连通分量（内部，支持屏蔽参数）
        static ComponentsResult computeComponents(
            const LGraph &graph,
            const std::string &blocked_vertex,
            const std::unordered_set<std::string> &blocked_edge_keys)
        {

            auto all_ids = graph.AllPlaceIds();
            std::unordered_set<std::string> visited;
            std::vector<int> sizes;

            for (const auto &id : all_ids)
            {
                if (id == blocked_vertex)
                    continue;
                if (visited.count(id))
                    continue;

                size_t before = visited.size();
                bfsComponent(graph, id, visited, blocked_vertex, blocked_edge_keys);
                int comp_size = static_cast<int>(visited.size() - before);
                sizes.push_back(comp_size);
            }

            // 按规模降序排列
            std::sort(sizes.begin(), sizes.end(), std::greater<int>());

            return ComponentsResult{static_cast<int>(sizes.size()), sizes};
        }

        // ==================== A. 连通分量分析 ====================

        ComponentsResult GetConnectedComponents(const LGraph &graph)
        {
            std::unordered_set<std::string> empty_set;
            return computeComponents(graph, "", empty_set);
        }

        // ==================== B. 最短路径 ====================

        PathResult GetShortestPath(const LGraph &graph,
                                   const std::string &from_id,
                                   const std::string &to_id,
                                   PathMode mode)
        {
            (void)graph;
            (void)from_id;
            (void)to_id;
            (void)mode;
            // TODO: 使用 Dijkstra 算法计算最短路径
            // 提示：
            //   1. 根据 mode 选择边权：DIST 使用 distance，TIME 使用 walk_time
            //   2. 只走 status == "open" 的边
            //   3. 使用优先队列（小顶堆）优化
            //   4. 维护 prev 数组以回溯完整路径
            //   5. 若不可达，返回 reachable = false 的 PathResult
            //   6. 路径中存储的是 place_id 序列（不是内部索引）
            std::cerr << "GetShortestPath 还没实现" << std::endl;
            return PathResult();
        }

        // ==================== B'. 时刻约束最短路径 ====================

        PathResult GetTimedShortestPath(const LGraph &graph,
                                        const std::string &from_id,
                                        const std::string &to_id,
                                        const std::string &time,
                                        PathMode mode)
        {
            (void)graph;
            (void)from_id;
            (void)to_id;
            (void)time;
            (void)mode;
            // TODO: 在给定时刻 time（HH:MM）下求最短路径
            // 提示：
            //   1. 判断某地点在 time 时是否开放：open_time <= time <= close_time（字符串比较即可）
            //   2. 只允许使用"此时开放"的地点作为路径上的节点（包括起点和终点）
            //   3. 只走 status == "open" 的边
            //   4. 余下和普通 Dijkstra 相同；可以复用 GetShortestPath 的实现思路
            //   5. 若起点或终点此时未开放，直接返回不可达
            std::cerr << "GetTimedShortestPath 还没实现" << std::endl;
            return PathResult();
        }

        // ==================== C. 必经点路径规划 ====================

        PathResult GetMustPassPath(const LGraph &graph,
                                   const std::string &from_id,
                                   const std::string &to_id,
                                   PathMode mode,
                                   const std::vector<std::string> &waypoints)
        {
            (void)graph;
            (void)from_id;
            (void)to_id;
            (void)mode;
            (void)waypoints;
            // TODO: 计算必经点路径
            // 提示：
            //   1. 将路径拆分为多段：from -> w1 -> w2 -> ... -> wk -> to
            //   2. 对每段调用 GetShortestPath
            //   3. 如果任意一段不可达，则整条路径不可达
            //   4. 拼接所有段的路径（注意去掉重复的中间点）
            //   5. 累加所有段的代价作为总代价
            std::cerr << "GetMustPassPath 还没实现" << std::endl;
            return PathResult();
        }

        // ==================== D. 最小生成树 ====================

        std::vector<EdgeNode> MinimumSpanningTree(const LGraph &graph)
        {
            (void)graph;
            // TODO: 计算最小生成树（在 Kruskal 与 Prim 中任选其一实现）
            //
            // 选项一：Kruskal + DSU
            //   1. 获取所有 status == "open" 的边，按 distance 升序排序
            //   2. 使用并查集 DSU 维护连通性
            //   3. 依次尝试加入边，若两端不在同一集合则加入 MST
            //
            // 选项二：Prim
            //   1. 从任一顶点出发，维护"已加入 MST 的顶点集合 U"
            //   2. 每次从 U 与剩余顶点之间的边中选距离最小的加入 MST
            //   3. 重复直至 U 包含所有顶点
            //
            // 共同收尾：若最终 MST 边数 != 顶点数 - 1（图不连通），返回空 vector
            //
            // 报告中需说明你选了哪种、为什么、复杂度是多少。
            std::cerr << "MinimumSpanningTree 还没实现" << std::endl;
            return {};
        }

        // ==================== E. 关键节点 / 关键边分析 ====================

        CriticalResult FindCriticalNodesAndEdges(const LGraph &graph)
        {
            (void)graph;
            // TODO: 找出"删去后会让图的连通分量数增加"的顶点和边
            //
            // 推荐方法（基于 PPT 第 3 章讲过的 BFS / DFS）：
            //   1. 先调用 GetConnectedComponents 算出原图的分量数 baseline
            //   2. 关键节点：对每个顶点 v
            //        a. 临时把 v 视作"被删除"（连同其所有相邻边一起忽略）
            //        b. 对剩余子图重新 BFS / DFS 计算连通分量数
            //        c. 若分量数 > baseline，则 v 是关键节点
            //   3. 关键边：对每条 status=="open" 的边 e
            //        a. 临时把 e 视作"closed"
            //        b. 重新计算连通分量数
            //        c. 若分量数 > baseline，则 e 是关键边
            //
            // 注意事项：
            //   - 仅考虑 status == "open" 的边和当前存在的顶点
            //   - 这种"临时删除"通常用一个"被屏蔽集合"参数控制 BFS/DFS 即可，
            //     不必真的修改图
            //   - 复杂度：O(V·(V+E)) + O(E·(V+E))
            //   - 千节点规模可接受；想优化的同学请看加分项中的 Tarjan 版本
            //
            // 加分提示：用 Tarjan 一遍 DFS 同时找全部关键节点和关键边
            //          （维护 dfn[] 和 low[]），可把复杂度降到 O(V+E)。
            //          若实现 Tarjan 版本请放在加分项里，**不要**替换本必做版本。
            std::cerr << "FindCriticalNodesAndEdges 还没实现" << std::endl;
            return CriticalResult{{}, {}};
        }
    }
}
