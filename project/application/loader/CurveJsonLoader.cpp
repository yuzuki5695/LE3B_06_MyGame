#include "CurveJsonLoader.h"
#include <fstream>
#include <stdexcept>

using namespace MyEngine;

namespace MyGame {

    using json = nlohmann::json;
    // 静的定数の定義
    const std::string CurveJsonLoader::kDefaultBaseDirectory = "Resources/Data/Camera/";
    const std::string CurveJsonLoader::kExtension = ".json";

    std::unique_ptr<BezierData> CurveJsonLoader::LoadFile(const std::string& fileName) {
        // 連結してフルパスを得る
        const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;
        // ファイルストリーム
        std::ifstream file;
        // ファイルを開く
        file.open(fullpath);
        // ファイルオープン失敗をチェック
        if (file.fail()) { assert(0); }
        // JSON文字列から解凍したデータ
        nlohmann::json deserialized;
        // 解凍
        file >> deserialized;
        // ファイルを閉じる
        std::unique_ptr<BezierData> bezierData = std::make_unique<BezierData>();
        // JSONの形式をチェック
        assert(deserialized.contains("object_names"));

        // JSONの形式をチェック
        for (const json& objectName : deserialized["object_names"]) {
            std::string curveName = objectName.get<std::string>();
            // 曲線名が存在するかチェック
            if (deserialized.contains(curveName)) {
                // 曲線データを再帰的に読み込む
                const json& curveJson = deserialized[curveName];
                // 曲線データの読み込み
                LoadBezierDataRecursive(curveJson, *bezierData);
            }
        }
        // ベジェデータを返す
        return bezierData;
    }

    void CurveJsonLoader::LoadBezierDataRecursive(const json& objectsJson, BezierData& bezierData) {
        assert(objectsJson.is_array());
		// JSON配列の各要素を処理
        for (const json& spline : objectsJson) {
			// "spline"が存在するかチェック
            for (const json& pointJson : spline) {
				// ベジェデータのポイントリストに新しいポイントを追加
                bezierData.points.emplace_back(BezierPoint{});
				// ポイントの参照を取得
                BezierPoint& point = bezierData.points.back();

                // "名前"が存在するかチェック
                if (pointJson.contains("name")) {
                    point.name = pointJson["name"].get<std::string>();
                }

				// "control_point"が存在するかチェック
                if (pointJson.contains("control_point")) {
                    const json& controlPoint = pointJson["control_point"];
                    // 読み込む
                    point.controlPoint.controlPoint = GetVector3FromJson(controlPoint);
                }

				// "handle_left"が存在するかチェック
                if (pointJson.contains("handle_left")) {
                    const json& handleLeft = pointJson["handle_left"];
					// 読み込む
                    point.controlPoint.handleLeft = GetVector3FromJson(handleLeft);
                }

                // "handle_right"が存在するかチェック
                if (pointJson.contains("handle_right")) {
                    const json& handleRight = pointJson["handle_right"];
					// 読み込む
                    point.controlPoint.handleRight = GetVector3FromJson(handleRight);
                }
            }
        }
    }

    Vector3 CurveJsonLoader::GetVector3FromJson(const json& jsonArray) {
		// JSON配列が3要素の数値であることを確認
        assert(jsonArray.is_array());
		// 要素数が3以上であることを確認（3要素以上あれば、最初の3要素を使用する）
        assert(jsonArray.size() >= 3);

        Vector3 result;
		// JSON配列の最初の3要素をVector3のx, y, zに割り当てる
        for (uint32_t i = 0; i < 3; i++) {
            result[i] = jsonArray[i].get<float>();
        }

        // 結果を返す
        return result;
    }
}