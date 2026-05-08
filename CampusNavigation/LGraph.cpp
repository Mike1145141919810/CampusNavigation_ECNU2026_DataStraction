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
        // TODO: 插入边
        //   - 两端顶点必须都存在，否则抛异常
        //   - 边已存在则抛 GraphException
        //   - 无向图中注意反向关联
        (void)from_id; (void)to_id; (void)distance; (void)walk_time; (void)status;
        throw GraphException("LGraph::InsertEdge 还没实现");
    }

    void LGraph::DeleteEdge(const std::string &from_id, const std::string &to_id) {
        // TODO: 删除边
        //   - 边不存在 → GraphException
        //   - 无向图中反向边同步删除
        (void)from_id; (void)to_id;
        throw GraphException("LGraph::DeleteEdge 还没实现");
    }

    void LGraph::UpdateEdge(const std::string &from_id, const std::string &to_id,
                            const std::string &field, const std::string &value) {
        // TODO: 按字段名更新边
        //   支持字段：distance, walk_time, status
        //   - distance / walk_time 需要转为 int
        //   - status 只能是 "open" 或 "closed"
        //   - 无向图中两个方向需同时更新
        (void)from_id; (void)to_id; (void)field; (void)value;
        throw GraphException("LGraph::UpdateEdge 还没实现");
    }

    EdgeNode LGraph::GetEdge(const std::string &from_id, const std::string &to_id) const {
        // TODO: 返回边的完整信息
        //   不存在 → GraphException
        (void)from_id; (void)to_id;
        throw GraphException("LGraph::GetEdge 还没实现");
    }

    // ==================== 道路状态 ====================

    void LGraph::CloseRoad(const std::string &from_id, const std::string &to_id) {
        // TODO: 将边 status 设为 "closed"（可复用 UpdateEdge）
        (void)from_id; (void)to_id;
        throw GraphException("LGraph::CloseRoad 还没实现");
    }

    void LGraph::OpenRoad(const std::string &from_id, const std::string &to_id) {
        // TODO: 将边 status 设为 "open"（可复用 UpdateEdge）
        (void)from_id; (void)to_id;
        throw GraphException("LGraph::OpenRoad 还没实现");
    }

    // ==================== 遍历 / 高级查询 ====================

    std::vector<std::string> LGraph::AllPlaceIds() const {
        // TODO: 返回当前图中所有存在的地点 id
        //   顺序由你决定（是否排序、按什么排序，请在报告中说明）
        return {};
    }

    std::vector<EdgeNode> LGraph::AllEdges(bool only_open) const {
        // TODO: 返回当前图中所有边
        //   - 无向图中每条边只出现一次
        //   - only_open = true 时仅返回 status == "open" 的边
        //   - 返回顺序由你决定
        (void)only_open;
        return {};
    }

    std::vector<EdgeNode> LGraph::GetAdjacentEdges(const std::string &place_id) const {
        // TODO: 返回某地点的所有邻接边完整信息
        //   place_id 不存在 → GraphException
        //   返回顺序由你决定
        (void)place_id;
        return {};
    }

    std::vector<std::string> LGraph::GetPlacesByCategory(const std::string &category) const {
        // TODO: 返回某类别下所有地点 id
        //   返回顺序由你决定
        (void)category;
        return {};
    }

}
