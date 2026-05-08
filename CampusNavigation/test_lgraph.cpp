//
// test_lgraph.cpp - LGraph 单元测试
// 编译：cmake --build build --config Debug
// 运行：.\build\test_lgraph.exe
//

#include "LGraph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>

using namespace Graph;

static int passed = 0;
static int failed = 0;

#define TEST(name) do { std::cout << "[TEST] " << name << " ... "; } while(0)
#define PASS() do { std::cout << "PASS" << std::endl; passed++; } while(0)
#define FAIL(msg) do { std::cout << "FAIL: " << msg << std::endl; failed++; } while(0)
#define CHECK(cond, msg) do { if (!(cond)) { FAIL(msg); return; } } while(0)

// ==================== 1. 基础操作 ====================

void test_vertex_crud() {
    TEST("Vertex CRUD (Insert/Get/Update/Delete)");
    LGraph g(false);

    LocationInfo v1("P0001", "Library", "Teaching", 30, "08:00", "22:00");
    LocationInfo v2("P0002", "Canteen", "Dining", 20, "07:00", "21:00");

    g.InsertVertex(v1);
    g.InsertVertex(v2);
    CHECK(g.VertexCount() == 2, "VertexCount should be 2");

    auto got = g.GetVertex("P0001");
    CHECK(got.display_name == "Library", "display_name mismatch");
    CHECK(got.category == "Teaching", "category mismatch");
    CHECK(got.stay_time == 30, "stay_time mismatch");
    CHECK(got.open_time == "08:00", "open_time mismatch");
    CHECK(got.close_time == "22:00", "close_time mismatch");

    // UpdateVertex
    g.UpdateVertex("P0001", "display_name", "Main Library");
    g.UpdateVertex("P0001", "category", "Education");
    g.UpdateVertex("P0001", "stay_time", "45");
    g.UpdateVertex("P0001", "open_time", "07:30");
    g.UpdateVertex("P0001", "close_time", "23:00");

    got = g.GetVertex("P0001");
    CHECK(got.display_name == "Main Library", "Update display_name failed");
    CHECK(got.category == "Education", "Update category failed");
    CHECK(got.stay_time == 45, "Update stay_time failed");
    CHECK(got.open_time == "07:30", "Update open_time failed");
    CHECK(got.close_time == "23:00", "Update close_time failed");

    // DeleteVertex
    g.DeleteVertex("P0002");
    CHECK(g.VertexCount() == 1, "VertexCount should be 1 after delete");
    CHECK(!g.exist_vertex("P0002"), "P0002 should not exist after delete");

    PASS();
}

void test_edge_crud() {
    TEST("Edge CRUD (Insert/Get/Update/Delete)");
    LGraph g(false);

    LocationInfo v1("P0001", "A", "Cat1", 0, "00:00", "23:59");
    LocationInfo v2("P0002", "B", "Cat2", 0, "00:00", "23:59");
    g.InsertVertex(v1);
    g.InsertVertex(v2);

    g.InsertEdge("P0001", "P0002", 100, 5, "open");
    CHECK(g.EdgesCount() == 1, "EdgesCount should be 1");
    CHECK(g.exist_edge("P0001", "P0002"), "edge P0001-P0002 should exist");
    CHECK(g.exist_edge("P0002", "P0001"), "reverse edge P0002-P0001 should exist (undirected)");

    auto e = g.GetEdge("P0001", "P0002");
    CHECK(e.distance == 100, "distance mismatch");
    CHECK(e.walk_time == 5, "walk_time mismatch");
    CHECK(e.status == "open", "status mismatch");

    // 验证反向边数据一致
    auto rev = g.GetEdge("P0002", "P0001");
    CHECK(rev.distance == 100, "reverse distance mismatch");
    CHECK(rev.walk_time == 5, "reverse walk_time mismatch");

    // UpdateEdge
    g.UpdateEdge("P0001", "P0002", "distance", "200");
    g.UpdateEdge("P0001", "P0002", "walk_time", "10");
    g.UpdateEdge("P0001", "P0002", "status", "closed");

    e = g.GetEdge("P0001", "P0002");
    CHECK(e.distance == 200, "Update distance failed");
    CHECK(e.walk_time == 10, "Update walk_time failed");
    CHECK(e.status == "closed", "Update status to closed failed");

    // 验证反向边同步
    rev = g.GetEdge("P0002", "P0001");
    CHECK(rev.distance == 200, "reverse distance sync failed");
    CHECK(rev.walk_time == 10, "reverse walk_time sync failed");
    CHECK(rev.status == "closed", "reverse status sync failed");

    // DeleteEdge
    g.DeleteEdge("P0001", "P0002");
    CHECK(g.EdgesCount() == 0, "EdgesCount should be 0 after delete");
    CHECK(!g.exist_edge("P0001", "P0002"), "edge should not exist after delete");
    CHECK(!g.exist_edge("P0002", "P0001"), "reverse edge should not exist after delete");

    PASS();
}

void test_road_status() {
    TEST("Road status (CloseRoad / OpenRoad)");
    LGraph g(false);

    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));
    g.InsertEdge("P0001", "P0002", 100, 5, "open");

    g.CloseRoad("P0001", "P0002");
    CHECK(g.GetEdge("P0001", "P0002").status == "closed", "CloseRoad failed");
    CHECK(g.GetEdge("P0002", "P0001").status == "closed", "CloseRoad reverse sync failed");

    g.OpenRoad("P0001", "P0002");
    CHECK(g.GetEdge("P0001", "P0002").status == "open", "OpenRoad failed");
    CHECK(g.GetEdge("P0002", "P0001").status == "open", "OpenRoad reverse sync failed");

    PASS();
}

// ==================== 2. 遍历/查询 ====================

void test_all_place_ids() {
    TEST("AllPlaceIds");
    LGraph g(false);
    g.InsertVertex(LocationInfo("P0003", "C", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));

    auto ids = g.AllPlaceIds();
    CHECK(ids.size() == 3, "should return 3 ids");

    std::sort(ids.begin(), ids.end());
    CHECK(ids[0] == "P0001", "first id should be P0001");
    CHECK(ids[1] == "P0002", "second id should be P0002");
    CHECK(ids[2] == "P0003", "third id should be P0003");

    PASS();
}

void test_all_edges() {
    TEST("AllEdges (undirected dedup, only_open filter)");
    LGraph g(false);

    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0003", "C", "", 0, "", ""));
    g.InsertEdge("P0001", "P0002", 100, 5, "open");
    g.InsertEdge("P0001", "P0003", 200, 10, "closed");

    // AllEdges(false) — 包含所有边
    auto all = g.AllEdges(false);
    CHECK(all.size() == 2, "should return 2 edges (undirected dedup)");

    // AllEdges(true) — 只返回 status=open
    auto open_only = g.AllEdges(true);
    CHECK(open_only.size() == 1, "should return 1 open edge");
    CHECK(open_only[0].distance == 100, "the open edge should be P0001-P0002");

    PASS();
}

void test_get_adjacent_edges() {
    TEST("GetAdjacentEdges");
    LGraph g(false);

    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0003", "C", "", 0, "", ""));
    g.InsertEdge("P0001", "P0002", 100, 5, "open");
    g.InsertEdge("P0001", "P0003", 200, 10, "closed");

    auto edges = g.GetAdjacentEdges("P0001");
    CHECK(edges.size() == 2, "P0001 should have 2 adjacent edges");

    edges = g.GetAdjacentEdges("P0002");
    CHECK(edges.size() == 1, "P0002 should have 1 adjacent edge");

    PASS();
}

void test_get_places_by_category() {
    TEST("GetPlacesByCategory");
    LGraph g(false);

    g.InsertVertex(LocationInfo("P0001", "Lib", "Teaching", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "Can", "Dining", 0, "", ""));
    g.InsertVertex(LocationInfo("P0003", "Gym", "Sports", 0, "", ""));
    g.InsertVertex(LocationInfo("P0004", "Lab", "Teaching", 0, "", ""));

    auto teaching = g.GetPlacesByCategory("Teaching");
    CHECK(teaching.size() == 2, "should find 2 Teaching places");

    auto dining = g.GetPlacesByCategory("Dining");
    CHECK(dining.size() == 1, "should find 1 Dining place");

    auto unknown = g.GetPlacesByCategory("Unknown");
    CHECK(unknown.empty(), "should find 0 for unknown category");

    PASS();
}

// ==================== 3. 异常测试 ====================

void test_exceptions() {
    TEST("Exceptions (duplicate / not found / invalid)");
    LGraph g(false);

    // duplicate vertex
    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    try {
        g.InsertVertex(LocationInfo("P0001", "B", "", 0, "", ""));
        FAIL("should throw on duplicate vertex");
    } catch (const GraphException &) {
        // expected
    }

    // get non-existent vertex
    try {
        g.GetVertex("P9999");
        FAIL("should throw on non-existent vertex");
    } catch (const GraphException &) {
        // expected
    }

    // delete non-existent vertex
    try {
        g.DeleteVertex("P9999");
        FAIL("should throw on delete non-existent vertex");
    } catch (const GraphException &) {
        // expected
    }

    // insert edge with non-existent vertex
    try {
        g.InsertEdge("P0001", "P9999", 100, 5, "open");
        FAIL("should throw on edge with non-existent vertex");
    } catch (const GraphException &) {
        // expected
    }

    // duplicate edge
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));
    g.InsertEdge("P0001", "P0002", 100, 5, "open");
    try {
        g.InsertEdge("P0001", "P0002", 200, 10, "open");
        FAIL("should throw on duplicate edge");
    } catch (const GraphException &) {
        // expected
    }

    // unknown field
    try {
        g.UpdateVertex("P0001", "unknown_field", "value");
        FAIL("should throw on unknown field");
    } catch (const GraphException &) {
        // expected
    }

    try {
        g.UpdateEdge("P0001", "P0002", "unknown_field", "value");
        FAIL("should throw on unknown edge field");
    } catch (const GraphException &) {
        // expected
    }

    // invalid status value
    try {
        g.UpdateEdge("P0001", "P0002", "status", "invalid");
        FAIL("should throw on invalid status");
    } catch (const GraphException &) {
        // expected
    }

    // get non-existent edge
    try {
        g.GetEdge("P0001", "P9999");
        FAIL("should throw on non-existent edge");
    } catch (const GraphException &) {
        // expected
    }

    PASS();
}

// ==================== 4. 有向图测试 ====================

void test_directed_graph() {
    TEST("Directed graph mode");
    LGraph g(true);

    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0003", "C", "", 0, "", ""));

    g.InsertEdge("P0001", "P0002", 100, 5, "open");
    CHECK(g.EdgesCount() == 1, "directed: EdgesCount should be 1");
    CHECK(g.exist_edge("P0001", "P0002"), "directed: forward edge should exist");
    CHECK(!g.exist_edge("P0002", "P0001"), "directed: reverse edge should NOT exist");

    // 有向图 DeleteVertex 也会清理入边
    g.InsertEdge("P0003", "P0001", 50, 2, "open");
    g.DeleteVertex("P0001");
    CHECK(g.EdgesCount() == 0, "after delete vertex, EdgesCount should be 0");
    CHECK(!g.exist_edge("P0003", "P0001"), "incoming edge should be cleaned up");

    PASS();
}

// ==================== 5. 真实数据加载 ====================

void load_real_data(LGraph &g, const std::string &places_path, const std::string &roads_path) {
    // 手动加载 places.csv（CsvIO 还没实现）
    std::ifstream pf(places_path);
    if (!pf.is_open()) {
        std::cerr << "  [WARN] Cannot open " << places_path << ", skip real data test" << std::endl;
        return;
    }

    std::string line;
    bool first = true;
    while (std::getline(pf, line)) {
        if (line.empty()) continue;
        // 跳过表头
        if (first) {
            first = false;
            if (line.find("place_id") != std::string::npos) continue;
        }
        // 兼容无表头时第一行也被跳过
        if (line.find("place_id") != std::string::npos) continue;

        for (auto &c : line) if (c == ',') c = ' ';
        std::istringstream iss(line);
        std::string pid, name, cat, st, ot, ct;
        if (!(iss >> pid >> name >> cat >> st >> ot >> ct)) continue;
        g.InsertVertex(LocationInfo(pid, name, cat, std::stoi(st), ot, ct));
    }
    pf.close();

    // 手动加载 roads.csv
    std::ifstream rf(roads_path);
    if (!rf.is_open()) {
        std::cerr << "  [WARN] Cannot open " << roads_path << ", skip real data test" << std::endl;
        return;
    }

    first = true;
    while (std::getline(rf, line)) {
        if (line.empty()) continue;
        if (first) {
            first = false;
            if (line.find("from_id") != std::string::npos) continue;
        }
        if (line.find("from_id") != std::string::npos) continue;

        for (auto &c : line) if (c == ',') c = ' ';
        std::istringstream iss(line);
        std::string fid, tid, dist, wt, stat;
        if (!(iss >> fid >> tid >> dist >> wt >> stat)) continue;
        g.InsertEdge(fid, tid, std::stoi(dist), std::stoi(wt), stat);
    }
    rf.close();
}

void test_real_data() {
    TEST("Real data from places.csv / roads.csv");
    LGraph g(false);

    load_real_data(g, "places.csv", "roads.csv");

    if (g.VertexCount() == 0) {
        std::cout << "SKIP (cannot open CSV files)" << std::endl;
        return;
    }

    CHECK(g.VertexCount() == 87, "places.csv should have 87 places");
    CHECK(g.EdgesCount() == 26, "roads.csv should have 26 roads");

    // 验证特定数据
    auto lib = g.GetVertex("P0018");
    CHECK(lib.display_name == "library", "P0018 should be library");
    CHECK(lib.category == "Teaching_Research_and_Administration", "library category");
    CHECK(lib.stay_time == 30, "library stay_time");

    // 验证边数据
    auto e = g.GetEdge("P0084", "P0025");
    CHECK(e.distance == 152, "P0084-P0025 distance");
    CHECK(e.walk_time == 2, "P0084-P0025 walk_time");

    // 验证无向图双向存储
    CHECK(g.exist_edge("P0025", "P0084"), "reverse edge P0025-P0084 should exist");

    // 验证 AllEdges 去重
    auto all_edges = g.AllEdges(false);
    CHECK(all_edges.size() == 26, "AllEdges should return 26 unique edges");

    // 验证分类查询
    auto dining = g.GetPlacesByCategory("Dining_room");
    CHECK(dining.size() == 3, "should have 3 dining rooms");

    // 验证邻接
    auto adj = g.GetAdjacentEdges("P0025");  // swimming_pool
    CHECK(adj.size() == 5, "P0025 should have 5 adjacent edges");

    // 删除一个顶点，验证完整性
    g.DeleteVertex("P0087");
    CHECK(g.VertexCount() == 86, "after delete P0087, should be 86 vertices");
    CHECK(!g.exist_vertex("P0087"), "P0087 should not exist");
    // P0087 连接：P0045(145,2), P0047(318,4), P0018(226,3)
    CHECK(!g.exist_edge("P0045", "P0087"), "edge P0045-P0087 should be cleaned");
    CHECK(!g.exist_edge("P0047", "P0087"), "edge P0047-P0087 should be cleaned");

    PASS();
}

// ==================== 6. 删除顶点时边计数测试 ====================

void test_edge_count_on_vertex_delete() {
    TEST("Edge count consistency after vertex delete");
    LGraph g(false);

    g.InsertVertex(LocationInfo("P0001", "A", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0002", "B", "", 0, "", ""));
    g.InsertVertex(LocationInfo("P0003", "C", "", 0, "", ""));
    g.InsertEdge("P0001", "P0002", 100, 5, "open");
    g.InsertEdge("P0001", "P0003", 200, 10, "open");

    CHECK(g.EdgesCount() == 2, "EdgesCount should be 2 before delete");

    g.DeleteVertex("P0001");
    CHECK(g.EdgesCount() == 0, "EdgesCount should be 0 after deleting P0001 (all edges gone)");

    // 剩余顶点应该还在
    CHECK(g.exist_vertex("P0002"), "P0002 should still exist");
    CHECK(g.exist_vertex("P0003"), "P0003 should still exist");

    PASS();
}

// ==================== 主函数 ====================

int main() {
    std::cout << "===== LGraph Phase 1 Tests =====" << std::endl << std::endl;

    // 1. 基础 CRUD
    test_vertex_crud();
    test_edge_crud();
    test_road_status();

    // 2. 遍历/查询
    test_all_place_ids();
    test_all_edges();
    test_get_adjacent_edges();
    test_get_places_by_category();

    // 3. 异常测试
    test_exceptions();

    // 4. 有向图
    test_directed_graph();

    // 5. 真实数据
    test_real_data();

    // 6. 边计数一致性
    test_edge_count_on_vertex_delete();

    std::cout << std::endl;
    std::cout << "===== Results: " << (passed + failed) << " tests, "
              << passed << " passed, " << failed << " failed =====" << std::endl;

    return failed > 0 ? 1 : 0;
}
