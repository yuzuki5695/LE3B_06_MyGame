#pragma once
#include <string>
#include <Vector3.h>
#include<vector>

// ベジェ曲線の制御点の構造体
struct BezierPoint {
    Vector3 handleLeft;
    Vector3 controlPoint;
    Vector3 handleRight;
};

// JSONファイルからベジェ曲線の制御点を読み込むクラス
class CurveJsonLoader {
public:
	// JSONファイルを読み込む
    std::vector<BezierPoint> LoadBezierFromJSON(const std::string& filePath);
};