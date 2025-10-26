#include "CurveJsonLoader.h"
#include <json.hpp>
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

std::vector<BezierPoint> CurveJsonLoader::LoadBezierFromJSON(const std::string& filePath) {
    std::vector<BezierPoint> points;

    // ファイルを開く
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("JSONファイルを開けませんでした");
    }

    // JSONデータを読み込む
    json j;
    file >> j;

    // object_names配列の存在チェック
    if (!j.contains("object_names") || !j["object_names"].is_array()) {
        throw std::runtime_error("object_names が見つかりません");
    }

    // 各オブジェクト名（カーブ名）ごとにループ
    for (const auto& name : j["object_names"]) {
        const std::string curveName = name.get<std::string>();

        // カーブデータの存在チェック
        if (!j.contains(curveName) || !j[curveName].is_array() || j[curveName].empty()) {
            continue;
        }

        // スプラインごとに処理
        for (const auto& spline : j[curveName]) {
            for (const auto& pointData : spline) {
                // control_point のみ存在する場合に対応
                BezierPoint pt;

                if (pointData.contains("control_point") && pointData["control_point"].is_array()) {
                    pt.controlPoint = {
                        pointData["control_point"][0].get<float>(),
                        pointData["control_point"][1].get<float>(),
                        pointData["control_point"][2].get<float>()
                    };
                }

                // handle_left / handle_right が存在する場合は読み込む
                if (pointData.contains("handle_left") && pointData["handle_left"].is_array()) {
                    pt.handleLeft = {
                        pointData["handle_left"][0].get<float>(),
                        pointData["handle_left"][1].get<float>(),
                        pointData["handle_left"][2].get<float>()
                    };
                }

                if (pointData.contains("handle_right") && pointData["handle_right"].is_array()) {
                    pt.handleRight = {
                        pointData["handle_right"][0].get<float>(),
                        pointData["handle_right"][1].get<float>(),
                        pointData["handle_right"][2].get<float>()
                    };
                }
                points.push_back(pt);
            }
        }
    }

    return points;
}
