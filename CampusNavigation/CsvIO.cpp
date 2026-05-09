//
// 动态图上的校园路线规划与设施分析系统
// CsvIO.cpp - CSV 文件读写实现
//

#include "CsvIO.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace Graph {
    namespace CsvIO {

        std::vector<LocationInfo> ReadPlaces(const std::string &path) {
            std::ifstream file(path);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file: " << path << std::endl;
                return {};
            }

            std::vector<LocationInfo> places;
            std::string line;
            bool first_line = true;

            while (std::getline(file, line)) {
                if (line.empty()) continue;

                if (first_line && line.compare(0, 8, "place_id") == 0) {
                    first_line = false;
                    continue;
                }
                first_line = false;

                // 将逗号替换为空格后使用 istringstream 解析
                for (char &c : line) {
                    if (c == ',') c = ' ';
                }

                std::istringstream iss(line);
                std::string place_id, display_name, category, open_time, close_time;
                int stay_time;

                if (!(iss >> place_id >> display_name >> category
                          >> stay_time >> open_time >> close_time)) {
                    std::cerr << "Warning: malformed line in " << path << std::endl;
                    continue;
                }

                places.emplace_back(place_id, display_name, category,
                                    stay_time, open_time, close_time);
            }

            return places;
        }

        std::vector<RoadRecord> ReadRoads(const std::string &path) {
            std::ifstream file(path);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file: " << path << std::endl;
                return {};
            }

            std::vector<RoadRecord> roads;
            std::string line;
            bool first_line = true;

            while (std::getline(file, line)) {
                if (line.empty()) continue;

                if (first_line && line.compare(0, 7, "from_id") == 0) {
                    first_line = false;
                    continue;
                }
                first_line = false;

                // 将逗号替换为空格后使用 istringstream 解析
                for (char &c : line) {
                    if (c == ',') c = ' ';
                }

                std::istringstream iss(line);
                RoadRecord r;

                if (!(iss >> r.from_id >> r.to_id >> r.distance
                          >> r.walk_time >> r.status)) {
                    std::cerr << "Warning: malformed line in " << path << std::endl;
                    continue;
                }

                roads.push_back(r);
            }

            return roads;
        }

        void WritePlaces(const std::string &path, const LGraph &graph) {
            std::ofstream file(path);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file for writing: " << path << std::endl;
                return;
            }

            file << "place_id,display_name,category,stay_time,open_time,close_time\n";

            auto ids = graph.AllPlaceIds();
            // 按 place_id 字典序输出，保持与测试数据格式一致
            std::sort(ids.begin(), ids.end());

            for (const auto &id : ids) {
                auto info = graph.GetVertex(id);
                file << info.place_id << ','
                     << info.display_name << ','
                     << info.category << ','
                     << info.stay_time << ','
                     << info.open_time << ','
                     << info.close_time << '\n';
            }
        }

        void WriteRoads(const std::string &path, const LGraph &graph) {
            std::ofstream file(path);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file for writing: " << path << std::endl;
                return;
            }

            file << "from_id,to_id,distance,walk_time,status\n";

            auto edges = graph.AllEdges(false);

            // 按 (from_id, to_id) 字典序排序，保持输出稳定
            std::sort(edges.begin(), edges.end(),
                      [](const EdgeNode &a, const EdgeNode &b) {
                          if (a.from_id != b.from_id) return a.from_id < b.from_id;
                          return a.to_id < b.to_id;
                      });

            for (const auto &e : edges) {
                file << e.from_id << ','
                     << e.to_id << ','
                     << e.distance << ','
                     << e.walk_time << ','
                     << e.status << '\n';
            }
        }
    }
}
