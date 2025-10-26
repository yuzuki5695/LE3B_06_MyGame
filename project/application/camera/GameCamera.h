#pragma once
#include <vector>
#include <memory>
#include <Camera.h>
#include <Vector3.h>
#include <CurveJsonLoader.h>
///====================================================
/// クォータニオン構造体（簡易版）
///====================================================
struct Quaternion {
    float w, x, y, z;
};


///====================================================
/// GameCameraクラス
/// <summary>
/// ベジェ曲線をもとにカメラを自動移動させるクラス
/// カーブデータ（制御点）はJSONファイルから読み込む
/// </summary>
///====================================================
class GameCamera {
public:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
    Vector3 LookAtRotation(const Vector3& forward);

    Quaternion ForwardToQuaternion(const Vector3& forward);
    Vector3 Slerp(const Vector3& v0, const Vector3& v1, float t);
private: // メンバ変数
    CurveJsonLoader* Jsondata = nullptr;         // ベジェ制御点を読み込むローダー
    Camera* camera_ = nullptr;                   // 実際に描画で使用されるカメラインスタンス
    Vector3 bezierPos_;                          // 現在のベジェ曲線上の位置
    std::vector<BezierPoint> bezierPoints;       // 移動に使う制御点データ
    bool movefige;                               // ベジェ曲線に沿って移動するフラグ      	
    float speed;                             // 移動速度
    Vector3 prevForward = { 0,0,1 };        // 前フレームの向きベクトル（回転補間用）        
    int currentSegment = 0; // レール上の現在のセグメントインデックス
public: // アクセッサ（Getter / Setter）
    // getter 
    Camera* Getcamera() { return camera_; }
    bool Getmovefige() { return movefige; }
    Vector3 GetbezierPos() { return bezierPos_; }
    // setter
    void Setmovefige(bool value) { movefige = value; }
    void SetbezierPos(Vector3 pos) { bezierPos_ = pos; }
    // 前方ベクトルを取得
    Vector3 GetForward() const {
        // 回転行列を使わずforwardを保持している場合はこちら
        return prevForward;
    }
    // BezierPoint の取得
    std::vector<BezierPoint>& GetBezierPoints() { return bezierPoints; }
    // すべての passed を ON/OFF
    void SetAllPassed(bool flag) { for (auto& point : bezierPoints) { point.passed = flag; } }
    // 一度通過した扱いにする（すべて true）
    void MarkAllAsPassed() { for (auto& point : bezierPoints) { if (!point.passed) point.passed = true; } }
};