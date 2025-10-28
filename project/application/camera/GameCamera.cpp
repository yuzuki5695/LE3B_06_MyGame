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
    // カメラの初期設定
    mode_ = ViewType::Main;

    speed = 0.2f;        // 1フレームあたり移動距離
    movefige = true;
    currentSegment = 0;

    // メインカメラ生成、初期化
    maincamera_ = new Camera;
    bezierPos_ = bezierPoints[0].controlPoint;
    maincamera_->SetTranslate(bezierPos_);
    prevForward = { 0, 0, 1 }; // 初期向き
    maincamera_->SetRotate(LookAtRotation(prevForward));
    // サブカメラ生成、初期化
    subcamera_ = new Camera;
    subcamera_->SetTranslate({ 2, 0, -3 });  // 定点視点の例
    subcamera_->SetRotate({ 0.0f, 0.0f, 0.0f });
    followInitialized_ = false;

    subOffset_ = {5.5f,-3.0f,10.0f};
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

    switch (mode_) {
    case ViewType::Main:
        if (CheckAndResumeMovement())
            UpdateBezierMovement();

        maincamera_->SetTranslate(bezierPos_);
        UpdateCameraRotation();
        break;
    case ViewType::Sub:
        if (!followTarget_) return;

    // ターゲット位置を常に毎フレーム参照
    {
        Vector3 targetPos = followTarget_->GetTranslate();
        UpdateSubCameraFollow(targetPos, subOffset_);
    }
        break;

    case ViewType::Transition:
        UpdateTransition();
        break;
    }

	//// カメラ更新
    maincamera_->Update();
    subcamera_->Update();
}

void GameCamera::SwitchView(ViewType targetType) {
    if (mode_ == ViewType::Transition || targetType == mode_)
        return; // 既に同じ or 切替中なら無視

    // メイン移動停止
    movefige = false;

    mode_ = ViewType::Transition;
    transitionTarget_ = targetType;
    transitionTimer_ = 0.0f;

    // 現在カメラ位置・回転を取得
    startPos_ = maincamera_->GetTranslate();
    startRot_ = maincamera_->GetRotate();

    // 目標位置・回転（サブ or メイン）を設定
    if (targetType == ViewType::Sub) {
            // --- 重要: subcamera をターゲット基準で初期化しておく ---
        if (followTarget_) {
            Vector3 targetPos = followTarget_->GetWorldPosition();
            Vector3 desiredPos = targetPos + subOffset_;
            subcamera_->SetTranslate(desiredPos);

            Vector3 dir = targetPos - desiredPos;
            if (Length(dir) > 0.0001f) {
                dir = Normalize(dir);
                float yaw = atan2(dir.x, dir.z);
                float pitch = -asin(dir.y);
                subcamera_->SetRotate({ pitch, yaw, 0.0f });
            }
        }

        endPos_ = subcamera_->GetTranslate();
        endRot_ = subcamera_->GetRotate();
        
        // 🟢 修正点: サブモード移行時は初期化を未完にする
        //followInitialized_ = false;

    } else {
        // サブ→メイン
        endPos_ = bezierPos_;
        // 現在のベジェ位置における正しいforwardを計算
        Vector3 targetForward;
        if (currentSegment < bezierPoints.size() - 1) {
            Vector3 next = bezierPoints[currentSegment + 1].controlPoint;
            Vector3 next2 = (currentSegment + 2 < bezierPoints.size()) ?
                bezierPoints[currentSegment + 2].controlPoint : next;
            targetForward = Normalize((next - bezierPos_) * 0.7f + (next2 - next) * 0.3f);
        } else {
            targetForward = prevForward;
        }
        endRot_ = LookAtRotation(targetForward);
    }
}

void GameCamera::UpdateTransition() {    
    // --- フレームごとの進行 ---
    float speedMultiplier = 1.0f;

    // 🎯 メイン → サブ のときだけ速度2倍
    if (transitionTarget_ == ViewType::Sub) {
        speedMultiplier = 5.0f;
    } 
    transitionTimer_ += (1.0f / 60.0f) * speedMultiplier;    
    float t = transitionTimer_ / transitionDuration_;

    if (t >= 1.0f) {
        t = 1.0f;
        mode_ = transitionTarget_; // 切替完了
        if (mode_ == ViewType::Main)
            movefige = true; // メインに戻るなら移動再開
    }

    // イージング（スムーズステップ）
    float easeT = t * t * (3 - 2 * t);

    // ---- 位置補間 ----
    Vector3 interpPos = startPos_ * (1 - easeT) + endPos_ * easeT;

    // ---- 回転補間（クォータニオンでSlerp）----
    Quaternion qStart = Quaternion::FromEuler(startRot_);
    Quaternion qEnd   = Quaternion::FromEuler(endRot_);
    Quaternion qInterp = Quaternion::Slerp(qStart, qEnd, easeT);
    Vector3 interpRot = qInterp.ToEuler();

    maincamera_->SetTranslate(interpPos);
    maincamera_->SetRotate(interpRot);
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

bool GameCamera::CheckAndResumeMovement() {
    if (!movefige) {
        // 再開条件：現在と次の制御点が「通過許可済み」
        if (bezierPoints[currentSegment].passed && bezierPoints[currentSegment].passed) {
            movefige = true;
            return true;
        } else {
            return false; // 許可が出るまで停止
        }
    }
    return true; // 通常進行OK
}

void GameCamera::UpdateBezierMovement() {
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
        bezierPos_ += Normalize(dir) * speed;
    }
}

void GameCamera::UpdateCameraRotation() {
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
    maincamera_->SetRotate(LookAtRotation(newForward));
    prevForward = newForward;
}

// GameCamera内
void GameCamera::UpdateSubCameraFollow(const Vector3& targetPos, const Vector3& offset) {
    if (!subcamera_) return;

    // --- カメラを固定位置に配置 ---
    subcamera_->SetTranslate(offset);

    // --- ターゲット方向を向く ---
    Vector3 toTarget = targetPos - offset;

    if (Length(toTarget) > 0.0001f) {
        toTarget = Normalize(toTarget);

        // Y軸（水平）回転
        float yaw = std::atan2(toTarget.x, toTarget.z);

        // X軸（垂直）回転
        float distanceXZ = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
        float pitch = -std::atan2(toTarget.y, distanceXZ);

        subcamera_->SetRotate({ pitch, yaw, 0.0f });
    }
}