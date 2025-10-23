#include "GameCamera.h"
#include <MatrixVector.h>

using namespace MatrixVector;

///====================================================
/// 初期化処理
///====================================================
void GameCamera::Initialize() {
    // JSONから制御点を読み込む
    Jsondata = new CurveJsonLoader();
    bezierPoints = Jsondata->LoadBezierFromJSON("Resources/levels/rail_curve_data.json");

    // 移動用パラメータ
    speed = 0.5f;        // Z方向の1フレームあたり移動量
    movefige = true;

    // カメラ生成
    camera_ = new Camera();
    camera_->SetTranslate(bezierPoints[0].controlPoint);
    camera_->SetRotate({0, 0, 0});
}

///====================================================
/// 更新処理
///====================================================
void GameCamera::Update() {
    if (!movefige || bezierPoints.size() < 2) return;

    // 現在位置を取得
    Vector3& pos = bezierPos_;
    if (pos == Vector3{}) pos = bezierPoints[0].controlPoint;

    // Z方向に speed だけ加算
    pos.z += speed;

    // 終点に到達したら停止
    if (pos.z >= bezierPoints[1].controlPoint.z) {
        pos.z = bezierPoints[1].controlPoint.z;
        movefige = false;
    }

    // X,Yは固定（0と5など）して Zだけ進む
    pos.x = bezierPoints[0].controlPoint.x;
    pos.y = bezierPoints[0].controlPoint.y;

    // カメラ更新
    camera_->SetTranslate(pos);

    // 常に Z方向に向ける
    Vector3 forward = {0.0f, 0.0f, 1.0f};
    camera_->SetRotate(LookAtRotation(forward));

    camera_->Update();
}

///====================================================
/// LookAt 用の回転計算（簡易版）
/// forward: 向きベクトル
///====================================================
Vector3 GameCamera::LookAtRotation(const Vector3& forward) {
    Vector3 rot;
    rot.y = atan2f(forward.x, forward.z); // Yaw
    rot.x = asinf(-forward.y);            // Pitch
    rot.z = 0.0f;                         // Roll
    return rot;
}
