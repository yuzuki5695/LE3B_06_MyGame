#include "GameCamera.h"
#include<MatrixVector.h>

using namespace MatrixVector;

///====================================================
/// 初期化処理
///====================================================
void GameCamera::Initialize() {
	// ベジェ曲線の制御点読み込み用インスタンスを生成
    Jsondata = new CurveJsonLoader();
    // jsonファイルからベジェ曲線の制御点を読み込む
    bezierPoints = Jsondata->LoadBezierFromJSON("Resources/levels/bezier.json");
	// 初期位置をベジェ曲線の始点に設定
    const BezierPoint& start = bezierPoints[segmentIndex];
    const BezierPoint& end = bezierPoints[segmentIndex + 1];
    bezierPos_ = BezierInterpolate(
        start.controlPoint,
        start.handleRight,
        end.handleLeft,
        end.controlPoint,
        t
    );

	// カメラ生成
    camera_ = new Camera();
    camera_->SetTranslate(bezierPos_);
    camera_->SetRotate({ 0, 0, 0 });
    // 
    movefige = false;
}

///====================================================
/// 更新化処理
///====================================================
void GameCamera::Update() {
    // カメラの位置を更新 
    camera_->SetTranslate(bezierPos_);
    camera_->Update();
}

///====================================================
/// ベジェ曲線の制御点を設定
///====================================================
Vector3 GameCamera::BezierInterpolate(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Vector3 result;
    // (1 - t)^3 * P0 : 始点の寄与（tが小さいほど影響大）
    result = uuu * p0;
    // 3 * (1 - t)^2 * t * P1 : 第1制御点の寄与（曲線の立ち上がり方向）
    result += 3 * uu * t * p1;
    // 3 * (1 - t) * t^2 * P2 : 第2制御点の寄与（曲線の終わり方向）
    result += 3 * u * tt * p2;
    // t^3 * P3 : 終点の寄与（tが1に近づくほど影響大）
    result += ttt * p3;

    return result;
}
///====================================================
/// ベジェ曲線に沿ったオブジェクト位置更新
///====================================================
void GameCamera::UpdateObjectPosition() {
    if (segmentIndex + 1 < bezierPoints.size())
    {
        // 速度は距離ベースで制御（0〜1000の範囲で距離を進める例）
        distanceAlongCurve += speed; // speedは距離単位で

        // 曲線長計算がまだなら初期化
        if (curveLengths.empty())
            CalculateCurveLength();

        // 距離を曲線長の範囲内に制限
        if (distanceAlongCurve > totalCurveLength)
        {
            distanceAlongCurve = totalCurveLength; // 最後の位置で止める
            movefige = false;                      // 移動を停止する
            bezierPos_ = bezierPoints.back().controlPoint; // 最後の点に固定
            return;
        }

        // 曲線長に対応するtを取得
        t = GetTForDistance(distanceAlongCurve);

        // tを元にベジェ曲線の位置計算
        const BezierPoint& start = bezierPoints[segmentIndex];
        const BezierPoint& end = bezierPoints[segmentIndex + 1];
        bezierPos_ = BezierInterpolate(
            start.controlPoint,
            start.handleRight,
            end.handleLeft,
            end.controlPoint,
            t
        );

        // 必要ならsegmentIndexの更新などの処理も検討（ここは曲線全体の長さ1本のときは不要）
    } else
    {
        // 最後の点に固定
        bezierPos_ = bezierPoints.back().controlPoint;
    }
}
///====================================================
/// 曲線長の計算
/// 曲線を細かく分割して長さ計算用のデータを作る（1回だけ呼ぶ）
///====================================================
void GameCamera::CalculateCurveLength(){
    const int steps = 1000; // 分割数（必要に応じて調整）
    curveLengths.clear();
    curveLengths.push_back(0.0f); // 始点の長さ0

    // 始点座標
    Vector3 prevPos = BezierInterpolate(
        bezierPoints[0].controlPoint,
        bezierPoints[0].handleRight,
        bezierPoints[1].handleLeft,
        bezierPoints[1].controlPoint,
        0.0f
    );

    float length = 0.0f;
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / steps;
         // 現在座標を計算
        Vector3 currPos = BezierInterpolate(
            bezierPoints[segmentIndex].controlPoint,
            bezierPoints[segmentIndex].handleRight,
            bezierPoints[segmentIndex + 1].handleLeft,
            bezierPoints[segmentIndex + 1].controlPoint,
            t
        );
        // 直線距離を加算
        length += Length(currPos - prevPos); // Length()はVector3の距離計算関数
        curveLengths.push_back(length);
        prevPos = currPos;
    }
    // 曲線全体の長さを保存
    totalCurveLength = length;
}

///====================================================
/// 距離に対応する補間係数tを取得
///====================================================
float GameCamera::GetTForDistance(float distance) const
{
    if (distance <= 0) return 0.0f;
    if (distance >= totalCurveLength) return 1.0f;

    // 曲線長配列から距離に最も近い区間を探す（二分探索など高速化可）
    int low = 0;
    int high = (int)curveLengths.size() - 1;

    while (low < high) {
        int mid = (low + high) / 2;
        if (curveLengths[mid] < distance) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    int idx = low;
    if (idx == 0) return 0.0f;

    // 区間内での線形補間
    float lengthBefore = curveLengths[idx - 1];
    float lengthAfter = curveLengths[idx];
    float segmentFraction = (distance - lengthBefore) / (lengthAfter - lengthBefore);

    float tBefore = (float)(idx - 1) / (curveLengths.size() - 1);
    float tAfter = (float)idx / (curveLengths.size() - 1);

    return tBefore + segmentFraction * (tAfter - tBefore);
}
