#pragma once
#include <string>
#include <Vector3.h>
#include <vector>

namespace MyGame {
    /// <summary>
    /// ベジェ曲線の1点分の制御情報を保持する構造体
    /// </summary>
    struct BezierPoint {
        std::string name;               // 制御点の名前
        MyEngine::Vector3 controlPoint; // 実際に通る点
        bool passed = false;          // この制御点を通過したかどうかのフラグ
    };

    /// <summary>
    /// JSONファイルからベジェ曲線制御点を読み込むクラス
    /// </summary>
    class CurveJsonLoader {
    public:
        /// <summary>
        /// 指定したパスのJSONファイルを読み込み、ベジェ制御点リストを返す
        /// </summary>
        /// <param name="filePath">JSONファイルのパス</param>
        /// <returns>BezierPointの配列</returns>s
        std::vector<std::vector<BezierPoint>> LoadBezierFromJSON(const std::string& filePath);
    };
}