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
        // TODO: 返回当前图中存在的顶点数
        return 0;
    }

    int LGraph::EdgesCount() const {
        // TODO: 返回当前图中存在的边数（无向图中每条边算一次）
        return 0;
    }

    bool LGraph::exist_vertex(const std::string &place_id) const {
        // TODO: 判断 place_id 对应的顶点是否存在
        (void)place_id;
        return false;
    }

    bool LGraph::exist_edge(const std::string &from_id, const std::string &to_id) const {
        // TODO: 判断 from_id 与 to_id 之间是否有边
        (void)from_id; (void)to_id;
        return false;
    }

    // ==================== 顶点操作 ====================

    void LGraph::InsertVertex(const LocationInfo &vertex_info) {
        // TODO: 插入顶点
        //   - 若 place_id 已存在，抛出 GraphException
        //   - 否则将其加入你的存储结构
        (void)vertex_info;
        throw GraphException("LGraph::InsertVertex 还没实现");
    }

    void LGraph::DeleteVertex(const std::string &place_id) {
        // TODO: 删除顶点，并同步删除所有与该顶点相关的边
        //   - 若 place_id 不存在，抛出 GraphException
        //   - 无向图中对端的邻接信息也要同步清理
        (void)place_id;
        throw GraphException("LGraph::DeleteVertex 还没实现");
    }

    void LGraph::UpdateVertex(const std::string &place_id,
                              const std::string &field, const std::string &value) {
        // TODO: 按字段名更新顶点信息
        //   支持的字段：display_name, category, stay_time, open_time, close_time
        //   - stay_time 需要将 value 转为 int
        //   - place_id 不存在 → GraphException
        //   - field 不支持 → GraphException
        (void)place_id; (void)field; (void)value;
        throw GraphException("LGraph::UpdateVertex 还没实现");
    }

    LocationInfo LGraph::GetVertex(const std::string &place_id) const {
        // TODO: 返回 place_id 对应的顶点完整信息
        //   不存在 → GraphException
        (void)place_id;
        throw GraphException("LGraph::GetVertex 还没实现");
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
