//
// 动态图上的校园路线规划与设施分析系统
// Algorithm.cpp - 图算法实现
//

#include "Algorithm.h"
#include <queue>
#include <limits>
#include <tuple>
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
            if (from_id == to_id) {
                return PathResult{0, {from_id}, true};
            }

            // dist[v] = 从 from 到 v 的最短距离
            std::unordered_map<std::string, int> dist;
            std::unordered_map<std::string, std::string> prev;

            // 小顶堆：(distance, place_id)
            using PQEntry = std::pair<int, std::string>;
            std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;

            dist[from_id] = 0;
            pq.push({0, from_id});

            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();

                if (d != dist[u]) continue;  // 陈旧条目，跳过

                if (u == to_id) break;  // 已找到最短路径

                auto edges = graph.GetAdjacentEdges(u);
                for (const auto &e : edges) {
                    if (e.status != "open") continue;

                    int w = (mode == DIST) ? e.distance : e.walk_time;
                    int nd = d + w;
                    const std::string &v = e.to_id;

                    auto it = dist.find(v);
                    if (it == dist.end() || nd < it->second ||
                        (nd == it->second && u < prev[v])) {
                        dist[v] = nd;
                        prev[v] = u;
                        pq.push({nd, v});
                    }
                }
            }

            // 判断是否可达
            if (dist.find(to_id) == dist.end()) {
                return PathResult();
            }

            // 回溯路径
            std::vector<std::string> path;
            std::string curr = to_id;
            while (curr != from_id) {
                path.push_back(curr);
                curr = prev[curr];
            }
            path.push_back(from_id);
            std::reverse(path.begin(), path.end());

            return PathResult{dist[to_id], path, true};
        }

        // ==================== B_plus. 分层图最短路径（拓展1） ====================

        PathResultK GetShortestPathK(const LGraph &graph,
                                      const std::string &from_id,
                                      const std::string &to_id,
                                      int K) {
            if (from_id == to_id) {
                PathResultK res;
                res.total_time = 0;
                res.k_used = 0;
                res.path = {from_id};
                res.reachable = true;
                return res;
            }

            const int INF = std::numeric_limits<int>::max();

            // dist[v][k] = 到达 v 恰好用 k 张券的最短时间
            std::unordered_map<std::string, std::unordered_map<int, int>> dist;
            // prev_node[v][k] = 前驱地点
            std::unordered_map<std::string, std::unordered_map<int, std::string>> prev_node;
            // prev_k_map[v][k] = 到达 (v,k) 时上一步的 k 值
            std::unordered_map<std::string, std::unordered_map<int, int>> prev_k_map;
            // coupon[v][k] = 到达 (v,k) 时是否在到达边上用了券
            std::unordered_map<std::string, std::unordered_map<int, bool>> coupon;

            // 优先队列：(time, place_id, k)
            using PQEntry = std::tuple<int, std::string, int>;
            std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;

            dist[from_id][0] = 0;
            pq.push({0, from_id, 0});

            int best_time = INF;
            int best_k = -1;

            while (!pq.empty()) {
                auto [time, u, k] = pq.top();
                pq.pop();

                // 陈旧条目
                auto it_u = dist.find(u);
                if (it_u == dist.end()) continue;
                auto it_uk = it_u->second.find(k);
                if (it_uk == it_u->second.end() || time != it_uk->second) continue;

                if (u == to_id) {
                    best_time = time;
                    best_k = k;
                    break;  // 第一次弹出终点即为最优
                }

                auto edges = graph.GetAdjacentEdges(u);
                for (const auto &e : edges) {
                    if (e.status != "open") continue;
                    const std::string &v = e.to_id;
                    int w = e.walk_time;

                    // 分支1：不用券
                    int nd = time + w;
                    if (!dist[v].count(k) || nd < dist[v][k]) {
                        dist[v][k] = nd;
                        prev_node[v][k] = u;
                        prev_k_map[v][k] = k;
                        coupon[v][k] = false;
                        pq.push({nd, v, k});
                    }

                    // 分支2：用券（k+1 ≤ K）
                    if (k + 1 <= K) {
                        int reduced = (w + 2) / 3;  // ceil(w / 3)
                        int nd2 = time + reduced;
                        if (!dist[v].count(k + 1) || nd2 < dist[v][k + 1]) {
                            dist[v][k + 1] = nd2;
                            prev_node[v][k + 1] = u;
                            prev_k_map[v][k + 1] = k;
                            coupon[v][k + 1] = true;
                            pq.push({nd2, v, k + 1});
                        }
                    }
                }
            }

            if (best_time == INF) {
                return PathResultK();
            }

            // 回溯路径和用券边
            std::vector<std::string> path;
            std::vector<std::pair<std::string, std::string>> fast;

            std::string curr = to_id;
            int ck = best_k;
            while (!(curr == from_id && ck == 0)) {
                path.push_back(curr);
                std::string pv = prev_node[curr][ck];
                int pk = prev_k_map[curr][ck];
                if (coupon[curr][ck]) {
                    fast.push_back({std::min(pv, curr), std::max(pv, curr)});
                }
                curr = pv;
                ck = pk;
            }
            path.push_back(from_id);
            std::reverse(path.begin(), path.end());
            std::reverse(fast.begin(), fast.end());

            PathResultK res;
            res.total_time = best_time;
            res.k_used = best_k;
            res.path = std::move(path);
            res.fast_edges = std::move(fast);
            res.reachable = true;
            return res;
        }

        // ==================== B'. 时刻约束最短路径 ====================

        PathResult GetTimedShortestPath(const LGraph &graph,
                                        const std::string &from_id,
                                        const std::string &to_id,
                                        const std::string &time,
                                        PathMode mode)
        {
            // 检查起止点是否在 time 时刻开放
            auto info_from = graph.GetVertex(from_id);
            if (time < info_from.open_time || time > info_from.close_time) {
                return PathResult();
            }
            auto info_to = graph.GetVertex(to_id);
            if (time < info_to.open_time || time > info_to.close_time) {
                return PathResult();
            }

            if (from_id == to_id) {
                return PathResult{0, {from_id}, true};
            }

            std::unordered_map<std::string, int> dist;
            std::unordered_map<std::string, std::string> prev;

            using PQEntry = std::pair<int, std::string>;
            std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;

            dist[from_id] = 0;
            pq.push({0, from_id});

            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();

                if (d != dist[u]) continue;
                if (u == to_id) break;

                auto edges = graph.GetAdjacentEdges(u);
                for (const auto &e : edges) {
                    if (e.status != "open") continue;

                    const std::string &v = e.to_id;

                    // 检查邻居 v 在 time 时刻是否开放
                    auto info_v = graph.GetVertex(v);
                    if (time < info_v.open_time || time > info_v.close_time) continue;

                    int w = (mode == DIST) ? e.distance : e.walk_time;
                    int nd = d + w;

                    auto it = dist.find(v);
                    if (it == dist.end() || nd < it->second ||
                        (nd == it->second && u < prev[v])) {
                        dist[v] = nd;
                        prev[v] = u;
                        pq.push({nd, v});
                    }
                }
            }

            if (dist.find(to_id) == dist.end()) {
                return PathResult();
            }

            std::vector<std::string> path;
            std::string curr = to_id;
            while (curr != from_id) {
                path.push_back(curr);
                curr = prev[curr];
            }
            path.push_back(from_id);
            std::reverse(path.begin(), path.end());

            return PathResult{dist[to_id], path, true};
        }

        // ==================== C. 必经点路径规划 ====================

        PathResult GetMustPassPath(const LGraph &graph,
                                   const std::string &from_id,
                                   const std::string &to_id,
                                   PathMode mode,
                                   const std::vector<std::string> &waypoints)
        {
            // 构建完整的点序列：from → w1 → w2 → ... → wk → to
            std::vector<std::string> stops;
            stops.push_back(from_id);
            for (const auto &wp : waypoints) {
                stops.push_back(wp);
            }
            stops.push_back(to_id);

            int total_cost = 0;
            std::vector<std::string> full_path;

            for (size_t i = 0; i + 1 < stops.size(); ++i) {
                auto seg = GetShortestPath(graph, stops[i], stops[i + 1], mode);
                if (!seg.reachable) {
                    return PathResult();  // 任一段不可达，整体不可达
                }

                total_cost += seg.total_cost;

                // 拼接路径：去掉段间重复的起点（即上一段的终点）
                if (full_path.empty()) {
                    full_path = seg.path;
                } else {
                    // seg.path[0] == full_path.back()（junction 点），跳过
                    for (size_t j = 1; j < seg.path.size(); ++j) {
                        full_path.push_back(seg.path[j]);
                    }
                }
            }

            return PathResult{total_cost, full_path, true};
        }

        // ==================== D. 最小生成树 ====================

        std::vector<EdgeNode> MinimumSpanningTree(const LGraph &graph)
        {
            auto ids = graph.AllPlaceIds();
            if (ids.empty()) return {};
            std::sort(ids.begin(), ids.end());

            // 从排序后的节点列表确定性收集边（避免 unordered_map 遍历顺序不确定）
            std::vector<EdgeNode> edges;
            std::unordered_set<std::string> seen_edges;
            for (const auto &id : ids) {
                auto adj = graph.GetAdjacentEdges(id);
                for (const auto &e : adj) {
                    if (e.status != "open") continue;
                    std::string ek = id < e.to_id ? (id + '|' + e.to_id) : (e.to_id + '|' + id);
                    if (seen_edges.count(ek)) continue;
                    seen_edges.insert(ek);
                    edges.push_back(e);
                }
            }

            // Kruskal: 按 distance 升序，等权按 (from,to) 字典序
            std::sort(edges.begin(), edges.end(),
                      [](const EdgeNode &a, const EdgeNode &b) {
                          if (a.distance != b.distance) return a.distance < b.distance;
                          std::string ak = std::min(a.from_id, a.to_id) + '|' + std::max(a.from_id, a.to_id);
                          std::string bk = std::min(b.from_id, b.to_id) + '|' + std::max(b.from_id, b.to_id);
                          return ak < bk;
                      });

            // Build id→index map
            std::unordered_map<std::string, int> idx;
            for (int i = 0; i < static_cast<int>(ids.size()); ++i) {
                idx[ids[i]] = i;
            }

            DSU dsu(static_cast<int>(ids.size()));
            std::vector<EdgeNode> mst;

            for (const auto &e : edges) {
                int u = idx[e.from_id];
                int v = idx[e.to_id];
                if (!dsu.same(u, v)) {
                    dsu.unite(u, v);
                    mst.push_back(e);
                }
            }

            // 不连通
            if (static_cast<int>(mst.size()) != static_cast<int>(ids.size()) - 1) {
                return {};
            }

            // 按 (min(u,v), max(u,v)) 字典序排序输出
            std::sort(mst.begin(), mst.end(),
                      [](const EdgeNode &a, const EdgeNode &b) {
                          std::string a_min = std::min(a.from_id, a.to_id);
                          std::string a_max = std::max(a.from_id, a.to_id);
                          std::string b_min = std::min(b.from_id, b.to_id);
                          std::string b_max = std::max(b.from_id, b.to_id);
                          if (a_min != b_min) return a_min < b_min;
                          return a_max < b_max;
                      });

            return mst;
        }

        // ==================== E. 关键节点 / 关键边分析 ====================

        CriticalResult FindCriticalNodesAndEdges(const LGraph &graph)
        {
            std::unordered_set<std::string> empty_set;

            // 基线：原图的连通分量数
            auto base = computeComponents(graph, "", empty_set);
            int baseline = base.count;

            CriticalResult result;

            // 关键节点：依次屏蔽每个顶点
            auto all_ids = graph.AllPlaceIds();
            for (const auto &id : all_ids) {
                auto comp = computeComponents(graph, id, empty_set);
                if (comp.count > baseline) {
                    result.critical_nodes.push_back(id);
                }
            }
            // 按 place_id 字典序排列
            std::sort(result.critical_nodes.begin(), result.critical_nodes.end());

            // 关键边：依次屏蔽每条 open 边
            auto open_edges = graph.AllEdges(true);
            for (const auto &e : open_edges) {
                std::string ek = (e.from_id < e.to_id)
                    ? (e.from_id + "|" + e.to_id)
                    : (e.to_id + "|" + e.from_id);
                std::unordered_set<std::string> blocked_edges = {ek};
                auto comp = computeComponents(graph, "", blocked_edges);
                if (comp.count > baseline) {
                    std::string u = std::min(e.from_id, e.to_id);
                    std::string v = std::max(e.from_id, e.to_id);
                    result.critical_edges.push_back({u, v});
                }
            }
            // 按 (min(u,v), max(u,v)) 字典序排列
            std::sort(result.critical_edges.begin(), result.critical_edges.end(),
                      [](const auto &a, const auto &b) {
                          if (a.first != b.first) return a.first < b.first;
                          return a.second < b.second;
                      });

            return result;
        }
    }
}
