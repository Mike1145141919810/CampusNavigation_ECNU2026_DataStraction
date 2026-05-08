//
// 动态图上的校园路线规划与设施分析系统
// LGraph.cpp - 图 ADT 实现（内部存储结构由你自行设计）
//

#include "LGraph.h"

namespace Graph {

    LGraph::LGraph(bool directed)
        : directed_(directed), edge_count_(0) {}

    // ==================== 基础信息 ====================

    int LGraph::VertexCount() const {
        return static_cast<int>(vertices_.size());
    }

    int LGraph::EdgesCount() const {
        return edge_count_;
    }

    bool LGraph::exist_vertex(const std::string &place_id) const {
        return vertices_.find(place_id) != vertices_.end();
    }

    bool LGraph::exist_edge(const std::string &from_id, const std::string &to_id) const {
        auto it_from = adj_.find(from_id);
        if (it_from == adj_.end()) return false;
        return it_from->second.find(to_id) != it_from->second.end();
    }

    // ==================== 顶点操作 ====================

    void LGraph::InsertVertex(const LocationInfo &vertex_info) {
        if (exist_vertex(vertex_info.place_id)) {
            throw GraphException("vertex already exists: " + vertex_info.place_id);
        }
        vertices_[vertex_info.place_id] = vertex_info;
    }

    void LGraph::DeleteVertex(const std::string &place_id) {
        auto it_v = vertices_.find(place_id);
        if (it_v == vertices_.end()) {
            throw GraphException("vertex not found: " + place_id);
        }

        // 先清理邻接边（无向图中需同步删除对端记录）
        auto it_adj = adj_.find(place_id);
        if (it_adj != adj_.end()) {
            if (!directed_) {
                for (const auto &kv : it_adj->second) {
                    auto &neighbor_adj = adj_[kv.first];
                    neighbor_adj.erase(place_id);
                    if (neighbor_adj.empty()) {
                        adj_.erase(kv.first);
                    }
                }
            }
            edge_count_ -= static_cast<int>(it_adj->second.size());
            adj_.erase(it_adj);
        }

        // 清理其他顶点指向该顶点的边（有向图场景）
        if (directed_) {
            for (auto &kv : adj_) {
                if (kv.second.erase(place_id) > 0) {
                    edge_count_--;
                }
            }
        }

        vertices_.erase(it_v);
    }

    void LGraph::UpdateVertex(const std::string &place_id,
                              const std::string &field, const std::string &value) {
        auto it = vertices_.find(place_id);
        if (it == vertices_.end()) {
            throw GraphException("vertex not found: " + place_id);
        }

        if (field == "display_name") {
            it->second.display_name = value;
        } else if (field == "category") {
            it->second.category = value;
        } else if (field == "stay_time") {
            it->second.stay_time = std::stoi(value);
        } else if (field == "open_time") {
            it->second.open_time = value;
        } else if (field == "close_time") {
            it->second.close_time = value;
        } else {
            throw GraphException("unknown field: " + field);
        }
    }

    LocationInfo LGraph::GetVertex(const std::string &place_id) const {
        auto it = vertices_.find(place_id);
        if (it == vertices_.end()) {
            throw GraphException("vertex not found: " + place_id);
        }
        return it->second;
    }

    // ==================== 边操作 ====================

    void LGraph::InsertEdge(const std::string &from_id, const std::string &to_id,
                            int distance, int walk_time, const std::string &status) {
        if (!exist_vertex(from_id)) {
            throw GraphException("vertex not found: " + from_id);
        }
        if (!exist_vertex(to_id)) {
            throw GraphException("vertex not found: " + to_id);
        }
        if (exist_edge(from_id, to_id)) {
            throw GraphException("edge already exists: " + from_id + " - " + to_id);
        }

        EdgeNode edge(from_id, to_id, distance, walk_time, status);
        adj_[from_id][to_id] = edge;
        edge_count_++;

        if (!directed_) {
            EdgeNode rev_edge(to_id, from_id, distance, walk_time, status);
            adj_[to_id][from_id] = rev_edge;
        }
    }

    void LGraph::DeleteEdge(const std::string &from_id, const std::string &to_id) {
        if (!exist_edge(from_id, to_id)) {
            throw GraphException("edge not found: " + from_id + " - " + to_id);
        }

        adj_[from_id].erase(to_id);
        if (adj_[from_id].empty()) {
            adj_.erase(from_id);
        }
        edge_count_--;

        if (!directed_) {
            adj_[to_id].erase(from_id);
            if (adj_[to_id].empty()) {
                adj_.erase(to_id);
            }
        }
    }

    void LGraph::UpdateEdge(const std::string &from_id, const std::string &to_id,
                            const std::string &field, const std::string &value) {
        if (!exist_edge(from_id, to_id)) {
            throw GraphException("edge not found: " + from_id + " - " + to_id);
        }

        auto update_field = [&](EdgeNode &e) {
            if (field == "distance") {
                e.distance = std::stoi(value);
            } else if (field == "walk_time") {
                e.walk_time = std::stoi(value);
            } else if (field == "status") {
                if (value != "open" && value != "closed") {
                    throw GraphException("invalid status: " + value);
                }
                e.status = value;
            } else {
                throw GraphException("unknown field: " + field);
            }
        };

        update_field(adj_[from_id][to_id]);

        if (!directed_) {
            EdgeNode &rev = adj_[to_id][from_id];
            // 用原始字段同步（保持 from_id / to_id 对称性）
            rev.distance = adj_[from_id][to_id].distance;
            rev.walk_time = adj_[from_id][to_id].walk_time;
            rev.status = adj_[from_id][to_id].status;
        }
    }

    EdgeNode LGraph::GetEdge(const std::string &from_id, const std::string &to_id) const {
        auto it_from = adj_.find(from_id);
        if (it_from != adj_.end()) {
            auto it_to = it_from->second.find(to_id);
            if (it_to != it_from->second.end()) {
                return it_to->second;
            }
        }
        throw GraphException("edge not found: " + from_id + " - " + to_id);
    }

    // ==================== 道路状态 ====================

    void LGraph::CloseRoad(const std::string &from_id, const std::string &to_id) {
        UpdateEdge(from_id, to_id, "status", "closed");
    }

    void LGraph::OpenRoad(const std::string &from_id, const std::string &to_id) {
        UpdateEdge(from_id, to_id, "status", "open");
    }

    // ==================== 遍历 / 高级查询 ====================

    std::vector<std::string> LGraph::AllPlaceIds() const {
        std::vector<std::string> ids;
        ids.reserve(vertices_.size());
        for (const auto &kv : vertices_) {
            ids.push_back(kv.first);
        }
        return ids;
    }

    std::vector<EdgeNode> LGraph::AllEdges(bool only_open) const {
        std::vector<EdgeNode> edges;
        for (const auto &from_kv : adj_) {
            for (const auto &to_kv : from_kv.second) {
                // 无向图每条边只输出一次（from_id < to_id 字典序）
                if (!directed_ && from_kv.first >= to_kv.first) {
                    continue;
                }
                if (only_open && to_kv.second.status != "open") {
                    continue;
                }
                edges.push_back(to_kv.second);
            }
        }
        return edges;
    }

    std::vector<EdgeNode> LGraph::GetAdjacentEdges(const std::string &place_id) const {
        if (!exist_vertex(place_id)) {
            throw GraphException("vertex not found: " + place_id);
        }
        std::vector<EdgeNode> edges;
        auto it = adj_.find(place_id);
        if (it != adj_.end()) {
            edges.reserve(it->second.size());
            for (const auto &kv : it->second) {
                edges.push_back(kv.second);
            }
        }
        return edges;
    }

    std::vector<std::string> LGraph::GetPlacesByCategory(const std::string &category) const {
        std::vector<std::string> ids;
        for (const auto &kv : vertices_) {
            if (kv.second.category == category) {
                ids.push_back(kv.first);
            }
        }
        return ids;
    }

}
