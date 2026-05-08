//
// 动态图上的校园路线规划与设施分析系统
// CsvIO.cpp - CSV 文件读写实现
//

#include "CsvIO.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Graph {
    namespace CsvIO {

        std::vector<LocationInfo> ReadPlaces(const std::string &path) {
            (void)path;
            // TODO: 从 CSV 读取地点信息
            // 提示：
            //   1. 打开文件，若失败则输出错误信息并返回空 vector
            //   2. 逐行读取，将逗号替换为空格后使用 istringstream 解析
            //   3. 若首行以 "place_id" 开头，则跳过（表头行）
            //   4. 跳过空行
            //   5. 每行解析为：place_id, display_name, category, stay_time, open_time, close_time
            //   6. 将解析结果加入 vector 并返回
            std::cerr << "CsvIO::ReadPlaces 还没实现" << std::endl;
            return {};
        }

        std::vector<RoadRecord> ReadRoads(const std::string &path) {
            (void)path;
            // TODO: 从 CSV 读取道路信息
            // 提示：
            //   1. 打开文件，若失败则输出错误信息并返回空 vector
            //   2. 逐行读取，将逗号替换为空格后使用 istringstream 解析
            //   3. 若首行以 "from_id" 开头，则跳过（表头行）
            //   4. 跳过空行
            //   5. 每行解析为：from_id, to_id, distance, walk_time, status
            //   6. 将解析结果加入 vector 并返回
            std::cerr << "CsvIO::ReadRoads 还没实现" << std::endl;
            return {};
        }

        void WritePlaces(const std::string &path, const LGraph &graph) {
            (void)path;
            (void)graph;
            // TODO: 将地点信息写回 CSV
            // 提示：
            //   1. 打开文件输出流
            //   2. 写入表头行：place_id,display_name,category,stay_time,open_time,close_time
            //   3. 通过 graph.AllPlaceIds() 获取所有地点 id，逐个调用 graph.GetVertex(id) 取完整信息
            //   4. 每行输出一个地点的完整信息，字段间用逗号分隔
            //   5. 关闭文件
            std::cerr << "CsvIO::WritePlaces 还没实现" << std::endl;
        }

        void WriteRoads(const std::string &path, const LGraph &graph) {
            (void)path;
            (void)graph;
            // TODO: 将道路信息写回 CSV
            // 提示：
            //   1. 打开文件输出流
            //   2. 写入表头行：from_id,to_id,distance,walk_time,status
            //   3. 使用 graph.AllEdges(false) 获取所有边（包括 closed）
            //   4. 输出每条边的 from_id, to_id, distance, walk_time, status
            //   5. 关闭文件
            std::cerr << "CsvIO::WriteRoads 还没实现" << std::endl;
        }
    }
}
