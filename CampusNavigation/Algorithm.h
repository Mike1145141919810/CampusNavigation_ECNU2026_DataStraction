//
// 动态图上的校园路线规划与设施分析系统
// Algorithm.h - 图算法接口
//

#ifndef CAMPUSNAVIGATION_ALGORITHM_H
#define CAMPUSNAVIGATION_ALGORITHM_H

#include "LGraph.h"
#include <vector>
#include <string>
#include <utility>

namespace Graph {
    namespace Algorithm {

        // ==================== 并查集 ====================
        class DSU {
        private:
            std::vector<int> parent, rank_;
        public:
            // 并查集构造函数，初始化 n 个元素
            explicit DSU(int n) {
                (void)n;
                // TODO: 初始化 parent 和 rank_ 数组
                // 提示：parent[i] = i, rank_[i] = 0
            }

            // 查找根节点并路径压缩
            int find(int x) {
                (void)x;
                // TODO: 实现带路径压缩的查找
                return -1;
            }

            // 合并两个集合（按秩合并）
            void unite(int x, int y) {
                (void)x;
                (void)y;
                // TODO: 实现按秩合并
            }

            // 检查两个节点是否属于同一集合
            bool same(int x, int y) {
                (void)x;
                (void)y;
                // TODO: 判断 x 和 y 是否在同一集合
                return false;
            }
        };

        // ==================== 路径模式枚举 ====================
        enum PathMode {
            DIST,   // 按距离最短
            TIME    // 按步行时间最短
        };

        // ==================== 最短路径结果 ====================
        struct PathResult {
            int total_cost;                     // 总代价（距离或时间）
            std::vector<std::string> path;      // 完整路径的 place_id 序列
            bool reachable;                     // 是否可达

            PathResult() : total_cost(0), reachable(false) {}
        };

        // ==================== 连通分量结果 ====================
        struct ComponentsResult {
            int count;                          // 连通分量个数
            std::vector<int> sizes;             // 每个连通分量的规模（降序排列）
        };

        // ==================== 关键节点 / 关键边分析结果 ====================
        struct CriticalResult {
            std::vector<std::string> critical_nodes;        // 关键节点（删去后连通分量数增加的顶点）
            std::vector<std::pair<std::string, std::string>> critical_edges;  // 关键边（删去后连通分量数增加的边）
        };

        // ==================== 算法函数 ====================

        // A. 连通分量分析
        // 计算当前图中（仅考虑 open 边）的连通分量
        // 返回：连通分量个数和各分量规模
        ComponentsResult GetConnectedComponents(const LGraph &graph);

        // B. 最短路径
        // 计算两点之间的最短路径，支持按距离或按时间
        // 参数：graph - 图，from_id/to_id - 起终点 place_id，mode - DIST 或 TIME
        // 返回：PathResult 包含总代价和完整路径
        // 仅考虑 status=open 的边
        PathResult GetShortestPath(const LGraph &graph,
                                   const std::string &from_id,
                                   const std::string &to_id,
                                   PathMode mode);

        // B'. 时刻约束最短路径
        // 在给定时刻 time（HH:MM）下计算两点间最短路径
        //   - 仅走 status=open 的边
        //   - 路径上所有地点（含起止点）必须在 time 时处于开放时段内
        //     （open_time <= time <= close_time，字符串比较即可）
        PathResult GetTimedShortestPath(const LGraph &graph,
                                        const std::string &from_id,
                                        const std::string &to_id,
                                        const std::string &time,
                                        PathMode mode);

        // C. 必经点路径规划
        // 给定起点、终点和一串必须按序经过的地点，求总路径
        // 实现思路：将问题拆成若干次单源最短路后拼接
        // 参数：waypoints - 必经点列表（按给定顺序）
        // 返回：PathResult 包含总代价和完整路径
        PathResult GetMustPassPath(const LGraph &graph,
                                   const std::string &from_id,
                                   const std::string &to_id,
                                   PathMode mode,
                                   const std::vector<std::string> &waypoints);

        // D. 最小生成树
        // 对当前所有 open 边构成的图计算 MST，权值按 distance 计算
        // 若图不连通，返回空 vector
        // 实现可在 Kruskal + DSU 与 Prim 中任选其一（PPT 第 4 章两种都讲了）
        // 返回：MST 中的边集合
        std::vector<EdgeNode> MinimumSpanningTree(const LGraph &graph);

        // E. 关键节点与关键边分析
        // 找出"删去后会让图的连通分量数增加"的顶点和边
        // （这等价于图论中的"割点"和"桥"，但本必做实现不要求 Tarjan）
        // 推荐方法：基于 PPT 第 3 章讲过的 BFS / DFS，对每个顶点 / 每条边
        //   - 临时把它从图中移除
        //   - 重算连通分量数
        //   - 比较是否增加
        // 仅考虑 status=open 的边
        // 复杂度：O(V·(V+E)) + O(E·(V+E))，对 1000 节点规模可接受
        //
        // （加分：用 Tarjan 一遍 DFS 同时找出全部关键节点和关键边，O(V+E)。见加分项 §一.4。）
        CriticalResult FindCriticalNodesAndEdges(const LGraph &graph);
    }
}
#endif //CAMPUSNAVIGATION_ALGORITHM_H
