#include "GameCamera.h"
#include <MatrixVector.h>
#include <algorithm> 

using namespace MatrixVector;

///====================================================
/// 初期化処理
///====================================================
void GameCamera::Initialize() {
    Jsondata = new CurveJsonLoader();
    bezierPoints = Jsondata->LoadBezierFromJSON("Resources/levels/bezier.json");

    speed = 0.2f;        // 1フレームあたり移動距離
    movefige = true;
    currentSegment = 0;

    camera_ = new Camera();
    bezierPos_ = bezierPoints[0].controlPoint;
    camera_->SetTranslate(bezierPos_);
    prevForward = {0, 0, 1}; // 初期向き
    camera_->SetRotate(LookAtRotation(prevForward));
}

///====================================================
/// 更新処理（複数制御点対応＋向き補間）
///====================================================
void GameCamera::Update() {
    if (bezierPoints.size() < 2) return;    
    // 範囲チェック（最後まで行ったら停止）
    if (currentSegment >= bezierPoints.size() - 1) {
        // 最後まで行ったら停止する場合：
        movefige = false;
        return;
    }

    // ---- 停止中でも再開条件を確認 ----
    if (!movefige) {
        // 次の制御点の「通過許可」が出ていれば再開
        if (bezierPoints[currentSegment].passed && bezierPoints[currentSegment].passed) {
            movefige = true; // ここで再開できる
        } else {
            return; // 許可が出るまで完全停止
        }
    }

    // 現在のセグメント start / end
    const Vector3& start = bezierPoints[currentSegment].controlPoint;
    const Vector3& end = bezierPoints[currentSegment + 1].controlPoint;

    // 移動ベクトル
    Vector3 dir = end - bezierPos_;
    float dist = Length(dir);

    if (dist <= speed) {
        // セグメント終了
        bezierPos_ = end;
        // 現在の制御点を通過済みに
        bezierPoints[currentSegment].passed = true;

        currentSegment++;
        if (currentSegment >= bezierPoints.size() - 1) {
            movefige = false;
            return;
        }

        // 次の制御点が未許可なら停止
        if (!bezierPoints[currentSegment].passed) {
            movefige = false;
            return;
        }
    } else {
        // 方向ベクトルに沿って speed 移動
        bezierPos_ +=  Normalize(dir) * speed;
    }

    // カメラ位置更新
    camera_->SetTranslate(bezierPos_);

    // === 向き補間（改良版） ===
    Vector3 targetForward;

    if (currentSegment < bezierPoints.size() - 1) {
        // 次の制御点とさらに次の制御点を使って、滑らかな接線方向を求める
        Vector3 next = bezierPoints[currentSegment + 1].controlPoint;
        Vector3 next2 = (currentSegment + 2 < bezierPoints.size()) ?
            bezierPoints[currentSegment + 2].controlPoint : next;

        // 現在→次の方向を0.7、次→次の次の方向を0.3混ぜる（曲がりを緩やかに）
        targetForward = Normalize((next - bezierPos_) * 0.7f + (next2 - next) * 0.3f);
    } else {
        targetForward = prevForward;
    }

    // 前回のforwardと今回のforwardの角度差を計算
    float dot = std::clamp(Dot(prevForward, targetForward), -1.0f, 1.0f);
    float angle = acosf(dot);

    // 曲がる角度が大きいほど補間を速く、小さいほどゆっくり
    float smooth = std::clamp(angle * 0.1f, 0.02f, 0.15f);

    // 球面線形補間（Slerp）で向きを滑らかに補間
    Vector3 newForward = Slerp(prevForward, targetForward, smooth);
    newForward = Normalize(newForward);

    // カメラ回転更新
    camera_->SetRotate(LookAtRotation(newForward));
    prevForward = newForward;

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
///====================================================
/// 球面線形補間 (Slerp)
///====================================================
Vector3 GameCamera::Slerp(const Vector3& v0, const Vector3& v1, float t) {
    float dot = Dot(v0, v1);
    dot = std::clamp(dot, -1.0f, 1.0f); // 安全クランプ

    float theta = acosf(dot) * t;
    Vector3 relative = Normalize(v1 - v0 * dot);
    return Normalize(v0 * cosf(theta) + relative * sinf(theta));
}
