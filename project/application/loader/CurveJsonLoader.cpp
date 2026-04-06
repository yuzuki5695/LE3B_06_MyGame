#include "CurveJsonLoader.h"
#include <json.hpp>
#include <fstream>
#include <stdexcept>

using namespace MyEngine;

namespace MyGame {

    using json = nlohmann::json;

    std::vector<std::vector<BezierPoint>> CurveJsonLoader::LoadBezierFromJSON(const std::string& filePath) {
        // 複数カーブ（スプライン）を格納する配列
        std::vector<std::vector<BezierPoint>> curves;

        // ファイルを開く
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON: " + filePath);
        }

        // JSONデータを読み込む
        json j;
        file >> j;

        // object_names の存在確認
        if (!j.contains("object_names") || !j["object_names"].is_array()) {
            throw std::runtime_error("object_names が見つかりません");
        }

        // カーブ名ごとに処理
        for (const json& name : j["object_names"]) {
            // カーブ名
            const std::string curveName = name.get<std::string>();
            // カーブデータが存在するかチェック
            if (!j.contains(curveName) || !j[curveName].is_array()) {
                continue;
            }
            // カーブごと
            for (const json& spline : j[curveName]) {
                // 1本のカーブを格納
                std::vector<BezierPoint> oneCurve;
                // 制御点ごとに処理
                for (const auto& pointData : spline) {
                    BezierPoint pt;

                    // name を読み込む
                    if (pointData.contains("name")) {
                        pt.name = pointData["name"].get<std::string>();
                    }

                    // control_point を読み込む
                    if (pointData.contains("control_point") &&
                        pointData["control_point"].is_array() &&
                        pointData["control_point"].size() == 3) {
                        pt.controlPoint = {
                            pointData["control_point"][0].get<float>(),
                            pointData["control_point"][1].get<float>(),
                            pointData["control_point"][2].get<float>()
                        };
                    }
                    // カーブに追加
                    oneCurve.push_back(pt);
                }
                // カーブ単位で追加
                if (!oneCurve.empty()) {
                    curves.push_back(oneCurve);
                }
            }
        }
        return curves;
    }
}