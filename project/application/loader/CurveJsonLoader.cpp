#include "CurveJsonLoader.h"
#include<json.hpp>
#include<fstream>

using json = nlohmann::json;

std::vector<BezierPoint> CurveJsonLoader::LoadBezierFromJSON(const std::string& filePath) {
    std::vector<BezierPoint> points;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("JSONファイルを開けませんでした");
    }

    json j;
    file >> j;

    if (!j.contains("object_names") || !j["object_names"].is_array()) {
        throw std::runtime_error("object_names が見つかりません");
    }

    for (const auto& name : j["object_names"]) {
        const std::string curveName = name;
        if (!j.contains(curveName) || !j[curveName].is_array() || j[curveName].empty()) {
            continue;
        }

        // スプライン単位でループ（通常は1スプラインだが拡張性のために）
        for (const auto& spline : j[curveName]) {
            for (const auto& pointData : spline) {
                BezierPoint pt;
                pt.handleLeft = {
                    pointData["handle_left"][0],
                    pointData["handle_left"][1],
                    pointData["handle_left"][2]
                };
                pt.controlPoint = {
                    pointData["control_point"][0],
                    pointData["control_point"][1],
                    pointData["control_point"][2]
                };
                pt.handleRight = {
                    pointData["handle_right"][0],
                    pointData["handle_right"][1],
                    pointData["handle_right"][2]
                };
                points.push_back(pt);            
            }
        }
    }

    return points;
}