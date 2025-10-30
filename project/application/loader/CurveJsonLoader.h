#pragma once
#include <string>
#include <Vector3.h>
#include<vector>

/// <summary>
/// ベジェ曲線の1点分の制御情報を保持する構造体
/// </summary>
struct BezierPoint {
    Vector3 handleLeft;
    Vector3 controlPoint;
    Vector3 handleRight;
    bool passed = true;  // ← この制御点を通過したか
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
    std::vector<BezierPoint> LoadBezierFromJSON(const std::string& filePath);
};