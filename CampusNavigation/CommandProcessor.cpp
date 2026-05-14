//
// 动态图上的校园路线规划与设施分析系统
// CommandProcessor.cpp - 命令解析与分发实现
//

#include "CommandProcessor.h"
#include <iostream>
#include <algorithm>
#include <fstream>

namespace Graph {

    bool CommandProcessor::ProcessCommand(const std::string &line) {
        (void)graph;

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) {
            return true;  // 空行，跳过
        }

        if (cmd == "QUIT") {
            return false;
        } else if (cmd == "LOAD") {
            cmdLoad(iss);
        } else if (cmd == "SAVE") {
            cmdSave(iss);
        } else if (cmd == "QUERY_PLACE") {
            cmdQueryPlace(iss);
        } else if (cmd == "QUERY_CATEGORY") {
            cmdQueryCategory(iss);
        } else if (cmd == "ADJ") {
            cmdAdj(iss);
        } else if (cmd == "ADD_PLACE") {
            cmdAddPlace(iss);
        } else if (cmd == "DELETE_PLACE") {
            cmdDeletePlace(iss);
        } else if (cmd == "UPDATE_PLACE") {
            cmdUpdatePlace(iss);
        } else if (cmd == "ADD_ROAD") {
            cmdAddRoad(iss);
        } else if (cmd == "DELETE_ROAD") {
            cmdDeleteRoad(iss);
        } else if (cmd == "UPDATE_ROAD") {
            cmdUpdateRoad(iss);
        } else if (cmd == "CLOSE_ROAD") {
            cmdCloseRoad(iss);
        } else if (cmd == "OPEN_ROAD") {
            cmdOpenRoad(iss);
        } else if (cmd == "COMPONENTS") {
            cmdComponents();
        } else if (cmd == "SHORTEST") {
            cmdShortest(iss);
        } else if (cmd == "TIMED_SHORTEST") {
            cmdTimedShortest(iss);
        } else if (cmd == "MUST_PASS") {
            cmdMustPass(iss);
        } else if (cmd == "MST") {
            cmdMst();
        } else if (cmd == "CRITICAL") {
            cmdCritical();
        } else {
            std::cout << "ERROR unknown_command" << std::endl;
        }

        return true;
    }

    // ==================== LOAD ====================
    void CommandProcessor::cmdLoad(std::istringstream &args) {
        std::string places_path, roads_path;
        if (!(args >> places_path >> roads_path)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        // 检查文件是否存在
        {
            std::ifstream test1(places_path);
            if (!test1.is_open()) {
                std::cout << "ERROR file_not_found" << std::endl;
                return;
            }
            std::ifstream test2(roads_path);
            if (!test2.is_open()) {
                std::cout << "ERROR file_not_found" << std::endl;
                return;
            }
        }

        // 清空旧图
        graph.Clear();

        // 加载地点
        auto places = CsvIO::ReadPlaces(places_path);
        try {
            for (const auto &p : places) {
                graph.InsertVertex(p);
            }
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        // 加载道路
        auto roads = CsvIO::ReadRoads(roads_path);
        try {
            for (const auto &r : roads) {
                graph.InsertEdge(r.from_id, r.to_id, r.distance, r.walk_time, r.status);
            }
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== SAVE ====================
    void CommandProcessor::cmdSave(std::istringstream &args) {
        std::string places_path, roads_path;
        if (!(args >> places_path >> roads_path)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        CsvIO::WritePlaces(places_path, graph);
        CsvIO::WriteRoads(roads_path, graph);

        std::cout << "OK" << std::endl;
    }

    // ==================== QUERY_PLACE ====================
    void CommandProcessor::cmdQueryPlace(std::istringstream &args) {
        std::string place_id;
        if (!(args >> place_id)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(place_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        auto info = graph.GetVertex(place_id);
        std::cout << "PLACE " << info.place_id << ' '
                  << info.display_name << ' '
                  << info.category << ' '
                  << info.stay_time << ' '
                  << info.open_time << ' '
                  << info.close_time << std::endl;
    }

    // ==================== QUERY_CATEGORY ====================
    void CommandProcessor::cmdQueryCategory(std::istringstream &args) {
        std::string category;
        if (!(args >> category)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        auto ids = graph.GetPlacesByCategory(category);
        std::sort(ids.begin(), ids.end());

        std::cout << "CATEGORY " << category << ' ' << ids.size();
        for (const auto &id : ids) {
            std::cout << ' ' << id;
        }
        std::cout << std::endl;
    }

    // ==================== ADJ ====================
    void CommandProcessor::cmdAdj(std::istringstream &args) {
        std::string place_id;
        if (!(args >> place_id)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(place_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        auto edges = graph.GetAdjacentEdges(place_id);
        // 按 neighbor (to_id) 字典序排序
        std::sort(edges.begin(), edges.end(),
                  [](const EdgeNode &a, const EdgeNode &b) {
                      return a.to_id < b.to_id;
                  });

        std::cout << "ADJ " << place_id << ' ' << edges.size();
        for (const auto &e : edges) {
            std::cout << ' ' << e.to_id << ':' << e.distance
                      << ':' << e.walk_time << ':' << e.status;
        }
        std::cout << std::endl;
    }

    // ==================== ADD_PLACE ====================
    void CommandProcessor::cmdAddPlace(std::istringstream &args) {
        std::string place_id, display_name, category, open_time, close_time;
        int stay_time;

        if (!(args >> place_id >> display_name >> category
                  >> stay_time >> open_time >> close_time)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (graph.exist_vertex(place_id)) {
            std::cout << "ERROR place_already_exists" << std::endl;
            return;
        }

        LocationInfo info(place_id, display_name, category,
                          stay_time, open_time, close_time);
        try {
            graph.InsertVertex(info);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== DELETE_PLACE ====================
    void CommandProcessor::cmdDeletePlace(std::istringstream &args) {
        std::string place_id;
        if (!(args >> place_id)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(place_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        try {
            graph.DeleteVertex(place_id);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== UPDATE_PLACE ====================
    void CommandProcessor::cmdUpdatePlace(std::istringstream &args) {
        std::string place_id, field, value;
        if (!(args >> place_id >> field >> value)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(place_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        // 校验字段合法性（stay_time 必须为整数）
        if (field == "stay_time") {
            try {
                (void)std::stoi(value);
            } catch (...) {
                std::cout << "ERROR invalid_field" << std::endl;
                return;
            }
        } else if (field != "display_name" && field != "category" &&
                   field != "open_time" && field != "close_time") {
            std::cout << "ERROR invalid_field" << std::endl;
            return;
        }

        try {
            graph.UpdateVertex(place_id, field, value);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== ADD_ROAD ====================
    void CommandProcessor::cmdAddRoad(std::istringstream &args) {
        std::string from_id, to_id, status;
        int distance, walk_time;

        if (!(args >> from_id >> to_id >> distance >> walk_time >> status)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(from_id) || !graph.exist_vertex(to_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        if (graph.exist_edge(from_id, to_id)) {
            std::cout << "ERROR road_already_exists" << std::endl;
            return;
        }

        try {
            graph.InsertEdge(from_id, to_id, distance, walk_time, status);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== DELETE_ROAD ====================
    void CommandProcessor::cmdDeleteRoad(std::istringstream &args) {
        std::string from_id, to_id;
        if (!(args >> from_id >> to_id)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_edge(from_id, to_id)) {
            std::cout << "ERROR road_not_found" << std::endl;
            return;
        }

        try {
            graph.DeleteEdge(from_id, to_id);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== UPDATE_ROAD ====================
    void CommandProcessor::cmdUpdateRoad(std::istringstream &args) {
        std::string from_id, to_id, field, value;
        if (!(args >> from_id >> to_id >> field >> value)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_edge(from_id, to_id)) {
            std::cout << "ERROR road_not_found" << std::endl;
            return;
        }

        // 校验字段合法性
        if (field == "distance" || field == "walk_time") {
            try {
                (void)std::stoi(value);
            } catch (...) {
                std::cout << "ERROR invalid_field" << std::endl;
                return;
            }
        } else if (field == "status") {
            if (value != "open" && value != "closed") {
                std::cout << "ERROR invalid_field" << std::endl;
                return;
            }
        } else {
            std::cout << "ERROR invalid_field" << std::endl;
            return;
        }

        try {
            graph.UpdateEdge(from_id, to_id, field, value);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== CLOSE_ROAD ====================
    void CommandProcessor::cmdCloseRoad(std::istringstream &args) {
        std::string from_id, to_id;
        if (!(args >> from_id >> to_id)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_edge(from_id, to_id)) {
            std::cout << "ERROR road_not_found" << std::endl;
            return;
        }

        try {
            graph.CloseRoad(from_id, to_id);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== OPEN_ROAD ====================
    void CommandProcessor::cmdOpenRoad(std::istringstream &args) {
        std::string from_id, to_id;
        if (!(args >> from_id >> to_id)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_edge(from_id, to_id)) {
            std::cout << "ERROR road_not_found" << std::endl;
            return;
        }

        try {
            graph.OpenRoad(from_id, to_id);
        } catch (const GraphException &e) {
            std::cout << "ERROR " << e.what() << std::endl;
            return;
        }

        std::cout << "OK" << std::endl;
    }

    // ==================== COMPONENTS ====================
    void CommandProcessor::cmdComponents() {
        auto result = Algorithm::GetConnectedComponents(graph);
        std::cout << "COMPONENTS " << result.count << " SIZES";
        for (int s : result.sizes) {
            std::cout << ' ' << s;
        }
        std::cout << std::endl;
    }

    // ==================== SHORTEST ====================
    void CommandProcessor::cmdShortest(std::istringstream &args) {
        std::string from_id, to_id, mode_str;
        if (!(args >> from_id >> to_id >> mode_str)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(from_id) || !graph.exist_vertex(to_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        Algorithm::PathMode mode = (mode_str == "TIME") ? Algorithm::TIME : Algorithm::DIST;
        auto result = Algorithm::GetShortestPath(graph, from_id, to_id, mode);

        if (!result.reachable) {
            std::cout << "NO_PATH" << std::endl;
            return;
        }

        std::cout << "PATH " << mode_str << ' ' << result.total_cost << " NODES";
        for (const auto &id : result.path) {
            std::cout << ' ' << id;
        }
        std::cout << std::endl;
    }

    // ==================== TIMED_SHORTEST ====================
    void CommandProcessor::cmdTimedShortest(std::istringstream &args) {
        std::string from_id, to_id, time, mode_str;
        if (!(args >> from_id >> to_id >> time >> mode_str)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(from_id) || !graph.exist_vertex(to_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        Algorithm::PathMode mode = (mode_str == "TIME") ? Algorithm::TIME : Algorithm::DIST;
        auto result = Algorithm::GetTimedShortestPath(graph, from_id, to_id, time, mode);

        if (!result.reachable) {
            std::cout << "NO_PATH" << std::endl;
            return;
        }

        std::cout << "PATH " << mode_str << ' ' << result.total_cost << " NODES";
        for (const auto &id : result.path) {
            std::cout << ' ' << id;
        }
        std::cout << std::endl;
    }

    // ==================== MUST_PASS ====================
    void CommandProcessor::cmdMustPass(std::istringstream &args) {
        std::string from_id, to_id, mode_str;
        int k;
        if (!(args >> from_id >> to_id >> mode_str >> k)) {
            std::cout << "ERROR invalid_arguments" << std::endl;
            return;
        }

        if (!graph.exist_vertex(from_id) || !graph.exist_vertex(to_id)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }

        std::vector<std::string> waypoints;
        for (int i = 0; i < k; ++i) {
            std::string wp;
            if (!(args >> wp)) {
                std::cout << "ERROR invalid_arguments" << std::endl;
                return;
            }
            if (!graph.exist_vertex(wp)) {
                std::cout << "ERROR place_not_found" << std::endl;
                return;
            }
            waypoints.push_back(wp);
        }

        Algorithm::PathMode mode = (mode_str == "TIME") ? Algorithm::TIME : Algorithm::DIST;
        auto result = Algorithm::GetMustPassPath(graph, from_id, to_id, mode, waypoints);

        if (!result.reachable) {
            std::cout << "NO_PATH" << std::endl;
            return;
        }

        std::cout << "PATH " << mode_str << ' ' << result.total_cost << " NODES";
        for (const auto &id : result.path) {
            std::cout << ' ' << id;
        }
        std::cout << std::endl;
    }

    // ==================== MST ====================
    void CommandProcessor::cmdMst() {
        auto mst = Algorithm::MinimumSpanningTree(graph);

        if (mst.empty()) {
            std::cout << "DISCONNECTED" << std::endl;
            return;
        }

        int total = 0;
        for (const auto &e : mst) {
            total += e.distance;
        }

        std::cout << "MST " << total << " EDGES";
        for (const auto &e : mst) {
            std::string u = std::min(e.from_id, e.to_id);
            std::string v = std::max(e.from_id, e.to_id);
            std::cout << ' ' << u << '-' << v << ':' << e.distance;
        }
        std::cout << std::endl;
    }

    // ==================== CRITICAL ====================
    void CommandProcessor::cmdCritical() {
        auto result = Algorithm::FindCriticalNodesAndEdges(graph);

        std::cout << "CRITICAL NODES " << result.critical_nodes.size();
        for (const auto &id : result.critical_nodes) {
            std::cout << ' ' << id;
        }

        std::cout << " EDGES " << result.critical_edges.size();
        for (const auto &e : result.critical_edges) {
            std::cout << ' ' << e.first << '-' << e.second;
        }
        std::cout << std::endl;
    }

}
