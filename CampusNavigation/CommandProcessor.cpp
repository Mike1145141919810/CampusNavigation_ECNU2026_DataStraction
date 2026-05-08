//
// 动态图上的校园路线规划与设施分析系统
// CommandProcessor.cpp - 命令解析与分发实现
//

#include "CommandProcessor.h"
#include <iostream>

namespace Graph {

    bool CommandProcessor::ProcessCommand(const std::string &line) {
        (void)graph;  // TODO: 各 cmd* 处理函数实现后会使用 graph。

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
        (void)args;
        // TODO: 实现 LOAD <places.csv> <roads.csv>
        // 提示：
        //   1. 从 args 读取两个文件路径
        //   2. 用 CsvIO::ReadPlaces 和 CsvIO::ReadRoads 读取数据
        //   3. 重建图（清空旧数据，插入新顶点和边）
        //   4. 成功输出 "OK"，失败输出 "ERROR <reason>"
        std::cerr << "cmdLoad 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== SAVE ====================
    void CommandProcessor::cmdSave(std::istringstream &args) {
        (void)args;
        // TODO: 实现 SAVE <places_out.csv> <roads_out.csv>
        // 提示：
        //   1. 从 args 读取两个输出文件路径
        //   2. 调用 CsvIO::WritePlaces 和 CsvIO::WriteRoads
        //   3. 成功输出 "OK"
        std::cerr << "cmdSave 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== QUERY_PLACE ====================
    void CommandProcessor::cmdQueryPlace(std::istringstream &args) {
        (void)args;
        // TODO: 实现 QUERY_PLACE <place_id>
        // 输出格式：PLACE <place_id> <display_name> <category> <stay_time> <open_time> <close_time>
        // 若不存在：ERROR place_not_found
        std::cerr << "cmdQueryPlace 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== QUERY_CATEGORY ====================
    void CommandProcessor::cmdQueryCategory(std::istringstream &args) {
        (void)args;
        // TODO: 实现 QUERY_CATEGORY <category>
        // 输出格式：CATEGORY <category> <count> <id1> <id2> ...
        // place_id 按字典序升序输出
        // 若无匹配：CATEGORY <category> 0
        std::cerr << "cmdQueryCategory 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== ADJ ====================
    void CommandProcessor::cmdAdj(std::istringstream &args) {
        (void)args;
        // TODO: 实现 ADJ <place_id>
        // 输出格式：ADJ <place_id> <count> <neighbor1>:<distance>:<walk_time>:<status> ...
        // 邻接点按 place_id 字典序升序输出
        // 若顶点不存在：ERROR place_not_found
        std::cerr << "cmdAdj 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== ADD_PLACE ====================
    void CommandProcessor::cmdAddPlace(std::istringstream &args) {
        (void)args;
        // TODO: 实现 ADD_PLACE <place_id> <display_name> <category> <stay_time> <open_time> <close_time>
        // 成功输出 "OK"
        // 若 place_id 已存在：ERROR place_already_exists
        std::cerr << "cmdAddPlace 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== DELETE_PLACE ====================
    void CommandProcessor::cmdDeletePlace(std::istringstream &args) {
        (void)args;
        // TODO: 实现 DELETE_PLACE <place_id>
        // 成功输出 "OK"
        // 若不存在：ERROR place_not_found
        std::cerr << "cmdDeletePlace 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== UPDATE_PLACE ====================
    void CommandProcessor::cmdUpdatePlace(std::istringstream &args) {
        (void)args;
        // TODO: 实现 UPDATE_PLACE <place_id> <field> <value>
        // 成功输出 "OK"
        // 支持的字段：display_name, category, stay_time, open_time, close_time
        // 若顶点不存在：ERROR place_not_found
        // 若字段不支持：ERROR invalid_field
        std::cerr << "cmdUpdatePlace 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== ADD_ROAD ====================
    void CommandProcessor::cmdAddRoad(std::istringstream &args) {
        (void)args;
        // TODO: 实现 ADD_ROAD <from_id> <to_id> <distance> <walk_time> <status>
        // 成功输出 "OK"
        // 若顶点不存在：ERROR place_not_found
        // 若边已存在：ERROR road_already_exists
        std::cerr << "cmdAddRoad 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== DELETE_ROAD ====================
    void CommandProcessor::cmdDeleteRoad(std::istringstream &args) {
        (void)args;
        // TODO: 实现 DELETE_ROAD <from_id> <to_id>
        // 成功输出 "OK"
        // 若边不存在：ERROR road_not_found
        std::cerr << "cmdDeleteRoad 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== UPDATE_ROAD ====================
    void CommandProcessor::cmdUpdateRoad(std::istringstream &args) {
        (void)args;
        // TODO: 实现 UPDATE_ROAD <from_id> <to_id> <field> <value>
        // 成功输出 "OK"
        // 支持的字段：distance, walk_time, status
        // 若边不存在：ERROR road_not_found
        // 若字段不支持：ERROR invalid_field
        std::cerr << "cmdUpdateRoad 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== CLOSE_ROAD ====================
    void CommandProcessor::cmdCloseRoad(std::istringstream &args) {
        (void)args;
        // TODO: 实现 CLOSE_ROAD <from_id> <to_id>
        // 成功输出 "OK"
        // 若边不存在：ERROR road_not_found
        std::cerr << "cmdCloseRoad 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== OPEN_ROAD ====================
    void CommandProcessor::cmdOpenRoad(std::istringstream &args) {
        (void)args;
        // TODO: 实现 OPEN_ROAD <from_id> <to_id>
        // 成功输出 "OK"
        // 若边不存在：ERROR road_not_found
        std::cerr << "cmdOpenRoad 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== COMPONENTS ====================
    void CommandProcessor::cmdComponents() {
        // TODO: 实现 COMPONENTS
        // 输出格式：COMPONENTS <count> SIZES <s1> <s2> ...
        // 规模按降序输出
        std::cerr << "cmdComponents 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== SHORTEST ====================
    void CommandProcessor::cmdShortest(std::istringstream &args) {
        (void)args;
        // TODO: 实现 SHORTEST <from_id> <to_id> <DIST|TIME>
        // 成功输出格式：PATH <DIST|TIME> <total_cost> NODES <id1> <id2> ...
        // 不可达输出：NO_PATH
        // 若顶点不存在：ERROR place_not_found
        std::cerr << "cmdShortest 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== TIMED_SHORTEST ====================
    void CommandProcessor::cmdTimedShortest(std::istringstream &args) {
        (void)args;
        // TODO: 实现 TIMED_SHORTEST <from_id> <to_id> <time> <DIST|TIME>
        // 含义：在给定时刻（HH:MM）下，找 from_id 到 to_id 的最短路径
        //   - 只走 status == "open" 的道路
        //   - 路径上（含起点、终点、中间点）所有地点必须在 time 时开放
        //     （即 open_time <= time <= close_time，字符串比较即可）
        // 成功输出格式：PATH <DIST|TIME> <total_cost> NODES <id1> <id2> ...
        // 不可达输出：NO_PATH
        // 若顶点不存在：ERROR place_not_found
        std::cerr << "cmdTimedShortest 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== MUST_PASS ====================
    void CommandProcessor::cmdMustPass(std::istringstream &args) {
        (void)args;
        // TODO: 实现 MUST_PASS <from_id> <to_id> <DIST|TIME> <k> <p1> <p2> ... <pk>
        // 成功输出格式：PATH <DIST|TIME> <total_cost> NODES <id1> <id2> ...
        // 不可达输出：NO_PATH
        // 若顶点不存在：ERROR place_not_found
        std::cerr << "cmdMustPass 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== MST ====================
    void CommandProcessor::cmdMst() {
        // TODO: 实现 MST
        // 成功输出格式：MST <total_distance> EDGES <u1>-<v1>:<w1> <u2>-<v2>:<w2> ...
        // 边按 (min(u,v), max(u,v)) 字典序排序输出
        // 图不连通输出：DISCONNECTED
        std::cerr << "cmdMst 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

    // ==================== CRITICAL ====================
    void CommandProcessor::cmdCritical() {
        // TODO: 实现 CRITICAL —— 输出关键节点与关键边
        // 输出格式：CRITICAL NODES <count> <id1> <id2> ... EDGES <count> <u1>-<v1> <u2>-<v2> ...
        //   - 关键节点（删去后连通分量数增加的顶点）按 place_id 字典序输出
        //   - 关键边（删去后连通分量数增加的边）按 (min(u,v), max(u,v)) 字典序输出
        // 实现可调用 Algorithm::FindCriticalNodesAndEdges(graph)
        std::cerr << "cmdCritical 还没实现" << std::endl;
        std::cout << "ERROR not_implemented" << std::endl;
    }

}
