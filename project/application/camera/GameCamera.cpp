#include "GameCamera.h"
#include <MatrixVector.h>
#include <algorithm> 

using namespace MatrixVector;

void GameCamera::AddSubCamera(const CameraTransform& trans) {
    // 新しいサブカメラインスタンスを生成
    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    // 渡されたCameraTransformを元に、位置と角度を設定
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);

    // サブカメラリストへ追加
    subcameras_.push_back(std::move(cam)); // 所有権を移動
}

void GameCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (const CameraTransform& trans : transforms) {
        // サブカメラを複数登録する
        AddSubCamera(trans);
    }
}

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

    subOffset_ = { 5.5f,-1.0f,15.0f };
    followInitialized_ = false;   
    prevForward = { 0, 0, 1 }; // 初期向き

    // メイン
    transform_ = { bezierPos_,LookAtRotation(prevForward) };
    // サブカメラ登録
    std::vector<CameraTransform> subCams = { { {2, 0, -3}, {0, 0, 0} } };
    // サブカメラを追加
   // AddSubCameras(subCams); 
}

///====================================================
/// 更新処理（複数制御点対応＋向き補間）
///====================================================
void GameCamera::Update() {
    if (bezierPoints.size() < 3) return;
    // 範囲チェック（最後まで行ったら停止）
    if (currentSegment >= bezierPoints.size() - 1) {
        // 最後まで行ったら停止する場合：
        movefige = false;
        return;
    }

    if (CheckAndResumeMovement())
        UpdateBezierMovement();
    transform_.translate = bezierPos_;
    UpdateCameraRotation();
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
    startPos_ = transform_.translate;
    startRot_ = transform_.rotate;
                
    std::unique_ptr<Camera>& subcam = subcameras_[0];
    // 目標位置・回転（サブ or メイン）を設定
    if (targetType == ViewType::Sub) {
            // --- 重要: subcamera をターゲット基準で初期化しておく ---
        if (followTarget_) {    
            Vector3 targetPos = followTarget_->GetWorldPosition();
            Vector3 desiredPos = targetPos + subOffset_;
            subcam->SetTranslate(desiredPos);

            Vector3 dir = targetPos - desiredPos;
            if (Length(dir) > 0.0001f) {
                dir = Normalize(dir);
                float yaw = atan2(dir.x, dir.z);
                float pitch = -asin(dir.y);
                subcam->SetRotate({ pitch, yaw, 0.0f });
            }
        }

        endPos_ = subcam->GetTranslate();
        endRot_ = subcam->GetRotate();
        
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

    transform_.translate = interpPos;
    transform_.rotate = interpRot;
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


    // --- 直線モード（start → point_01） ---
    if (currentSegment == 0) {
        Vector3 dir = end - bezierPos_;
        float dist = Length(dir);

        if (dist <= speed) {
            bezierPos_ = end;
            bezierPoints[currentSegment].passed = true;
            currentSegment++;
        } else {
            bezierPos_ += Normalize(dir) * speed; // ベクトル直進
        }
        return;
    }


    // --- 補完モード（それ以降） ---
    t_ += speed * 0.01f; // イージング進行速度（調整可能）

    if (t_ >= 1.0f) {
        t_ = 0.0f;
        currentSegment++;
        bezierPos_ = end;

        if (currentSegment >= bezierPoints.size() - 1) {
            movefige = false;
            return;
        }
    } else {
        // 次セグメントの制御点群（前後を参照して曲線化）
        Vector3 p0 = (currentSegment > 0) ? bezierPoints[currentSegment - 1].controlPoint : start;
        Vector3 p1 = start;
        Vector3 p2 = end;
        Vector3 p3 = (currentSegment + 2 < bezierPoints.size()) ?
            bezierPoints[currentSegment + 2].controlPoint : end;

        // Cubic Catmull-Rom スプライン補間（滑らかに繋がる）
        bezierPos_ = CatmullRom(p0, p1, p2, p3, t_);
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
    transform_.rotate = LookAtRotation(newForward);
    prevForward = newForward;
}

// GameCamera内
void GameCamera::UpdateSubCameraFollow(const Vector3& targetPos, const Vector3& offset) {
    std::unique_ptr<Camera>& subcam = subcameras_[0];
    if (!subcam) return;
        // --- 正: ワールド位置 = ターゲット位置 + オフセット ---
    Vector3 worldPos = targetPos + offset;
    // subcam カメラを固定位置に配置 ---
    subcam->SetTranslate(worldPos);

    // --- ターゲット方向を向く ---
    Vector3 toTarget = targetPos - worldPos;

    if (Length(toTarget) > 0.0001f) {
        toTarget = Normalize(toTarget);

        // Y軸（水平）回転
        float yaw = std::atan2(toTarget.x, toTarget.z);

        // X軸（垂直）回転
        float distanceXZ = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
        float pitch = -std::atan2(toTarget.y, distanceXZ);

        subcam->SetRotate({ pitch, yaw, 0.0f });
    }
}

Vector3 GameCamera::CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) * t2 +
        (-p0 + 3.0f*p1 - 3.0f*p2 + p3) * t3
    );
}