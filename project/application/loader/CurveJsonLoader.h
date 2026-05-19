#pragma once
#include <string>
#include <Vector3.h>
#include <vector>
#include <json.hpp>

namespace MyGame {
    /// <summary>
	/// ベジェ曲線の1点分の制御情報を保持する構造体
    /// </summary>
    struct ControlPoint {
        MyEngine::Vector3 handleLeft;  // ベジェ曲線の左側のハンドル（制御点）
        MyEngine::Vector3 controlPoint;// ベジェ曲線の制御点（実際の位置）
        MyEngine::Vector3 handleRight; // ベジェ曲線の右側のハンドル（制御点）
    };

    /// <summary>
	/// ベジェ曲線の1点分のデータを保持する構造体
    /// </summary>
    struct BezierPoint {
        std::string name;             // 制御点の名前
        ControlPoint controlPoint;    // 制御点の情報
        bool passed = false;          // この制御点を通過したかどうかのフラグ
    };
    
    /// <summary>
	/// ベジェ曲線の全制御点を保持する構造体
    /// </summary>
    struct BezierData {
        std::vector<BezierPoint> points;
    };

    /// <summary>
    /// JSONファイルからベジェ曲線制御点を読み込むクラス
    /// </summary>
    class CurveJsonLoader {
    public: // 静的定数
        static const std::string kDefaultBaseDirectory;
        static const std::string kExtension;
    public: // メンバ関数
        /// <summary>
        /// ファイルの読み込み
        /// </summary>
        /// <param name="fileName"></param>
        /// <returns></returns>
        std::unique_ptr<BezierData> LoadFile(const std::string& fileName);
        /// <summary>
        /// JSONデータからベジェ曲線の制御点を再帰的に読み込む関数
        /// </summary>
        /// <param name="objectsJson"></param>
        /// <param name="levelData"></param>
        void LoadBezierDataRecursive(const nlohmann::json& objectsJson, BezierData& levelData);
    private: // 内部関数
        /// <summary>
		/// JSON配列からVector3を取得する関数
        /// </summary>
        /// <param name="jsonArray"></param>
        /// <returns></returns>
        MyEngine::Vector3 GetVector3FromJson(const nlohmann::json& jsonArray);
    private: // メンバ変数
		// ベジェ曲線の制御点データを保持する変数
        std::unique_ptr<BezierData> bezierdata_;
    };
}