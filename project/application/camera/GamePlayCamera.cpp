#include "GamePlayCamera.h"
#include<MatrixVector.h>
#include<algorithm> 
#include<CameraManager.h>
#include<MathUtil.h>

using namespace MathUtil;
using namespace MatrixVector;

///====================================================
/// 初期化処理
///====================================================
void GamePlayCamera::Initialize() {
    Jsondata = new CurveJsonLoader();
    bezierPoints = Jsondata->LoadBezierFromJSON("Resources/levels/bezier.json");
    // カメラの初期設定
    //mode_ = ViewType::Main;

    speed = 0.3f;        // 1フレームあたり移動距離
    movefige = true;
    currentSegment = 0;

    // 初期位置を最初の制御点に合わせる
    bezierPos_ = bezierPoints[0].controlPoint;
    prevForward = { 0, 0, 1 }; // 初期向き
    // メイン
    transform_ = { bezierPos_,LookAtRotation(prevForward) };
    // サブカメラ登録 
    std::vector<CameraTransform> subCams = { { {2, 0, 50}, {0, 0, 0} } };
    // サブカメラを追加
    AddSubCameras(subCams);

    // メインカメラは追従モードにする
    CameraManager::GetInstance()->SetMode(CameraMode::Follow);

    followInitialized_ = false;
    subOffset_ = { 5.5f,-1.0f,15.0f };

    railInfo_.totalLength = 0.0f;
    railInfo_.segmentLengths.clear();

    for (size_t i = 0; i + 1 < bezierPoints.size(); ++i) {
        float len = Length(
            bezierPoints[i + 1].controlPoint -
            bezierPoints[i].controlPoint
        );
        railInfo_.segmentLengths.push_back(len);
        railInfo_.totalLength += len;
    }

    currentRailLength_ = 0.0f;
    prevPos_ = bezierPos_;
}

///====================================================
/// 更新処理（複数制御点対応＋向き補間）
///====================================================
void GamePlayCamera::Update() {
    if (bezierPoints.size() < 3) return;
    // 範囲チェック（最後まで行ったら停止）
    if (currentSegment >= bezierPoints.size() - 1) {
        // 最後まで行ったら停止する場合：
        movefige = false;
        return;
    }

    if (CameraManager::GetInstance()->GetMode() == CameraMode::Transition) {
//        UpdateTransition();

        CameraManager::GetInstance()->SetTypeview(ViewCameraType::Sub);


    } else if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Main && CameraManager::GetInstance()->GetMode() == CameraMode::Follow) {
        if (CheckAndResumeMovement())
            UpdateBezierMovement();
        transform_.translate = bezierPos_;
        UpdateCameraRotation();
    } else if (CameraManager::GetInstance()->GetTypeview() == ViewCameraType::Sub) {     // サブカメラの処理
        UpdateSubCameraFollow();
    }


}

bool GamePlayCamera::CheckAndResumeMovement() {
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


void GamePlayCamera::UpdateBezierMovement() {
    // 現在のセグメント start / end
    const Vector3& start = bezierPoints[currentSegment].controlPoint;
    const Vector3& end = bezierPoints[currentSegment + 1].controlPoint;
    Vector3 oldPos = bezierPos_;

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
            currentRailLength_ += Length(Normalize(dir) * speed); // ★追加
        }
        return;
    }

    // --- 補完モード（それ以降） ---
    // 現在のセグメント長を取得（Catmull-Rom補間を距離基準で進めるため）
    Vector3 p0 = (currentSegment > 0) ? bezierPoints[currentSegment - 1].controlPoint : start;
    Vector3 p1 = start;
    Vector3 p2 = end;
    Vector3 p3 = (currentSegment + 2 < bezierPoints.size()) ? bezierPoints[currentSegment + 2].controlPoint : end;
  
    // 簡易的にセグメント長を「直線距離」で近似
    float segmentLength = Length(p2 - p1);
    if (segmentLength < 0.0001f) segmentLength = 0.0001f;

    // t の増分を距離基準で計算
    float deltaT = speed / segmentLength;
    t_ += deltaT;

    if (t_ >= 1.0f) {
        t_ = 0.0f;
        currentSegment++;
        bezierPos_ = end;

        if (currentSegment >= bezierPoints.size() - 1) {
            movefige = false;
            return;
        }
    } else {
        // Catmull-Rom 補間
        bezierPos_ = CatmullRom(p0, p1, p2, p3, t_);
    }
    // 移動距離を累積
    currentRailLength_ += Length(bezierPos_ - oldPos);
    prevPos_ = bezierPos_;
}

void GamePlayCamera::UpdateCameraRotation() {
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

    prevForward = newForward;

    // ---------- forward 更新 ----------
    forward_ = newForward;

    // ---------- right / up を更新 ----------
    // 世界の上方向（Y軸）
    Vector3 worldUp = { 0,1,0 };

    // right = forward × worldUp
    right_ = Normalize(Cross(worldUp, forward_));

    // up = right × forward（直交化）
    up_ = Normalize(Cross(forward_, right_));
 
    // カメラの transform.rotate 更新
    transform_.rotate = LookAtRotation(forward_);
}

Vector3 GamePlayCamera::CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) * t2 +
        (-p0 + 3.0f*p1 - 3.0f*p2 + p3) * t3
    );
}

void GamePlayCamera::AddSubCamera(const CameraTransform& trans) {
    // 新しいサブカメラインスタンスを生成
    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    // 渡されたCameraTransformを元に、位置と角度を設定
    cam->SetTranslate(trans.translate);
    cam->SetRotate(trans.rotate);

    // サブカメラリストへ追加
    subcameras_.push_back(std::move(cam)); // 所有権を移動
}

void GamePlayCamera::AddSubCameras(const std::vector<CameraTransform>& transforms) {
    for (const CameraTransform& trans : transforms) {
        // サブカメラを複数登録する
        AddSubCamera(trans);
    }
}

void GamePlayCamera::UpdateTransition() {
    auto* cm = CameraManager::GetInstance();

    Camera* mainCam = cm->GetMainCamera();
    Camera* subCam = cm->GetActiveCamera();

    if (!mainCam || !subCam || !followTarget_) return;

    // 初回だけ初期位置を保存
    if (!isTransitioning_) {
        isTransitioning_ = true;

        mainStartPos_ = mainCam->GetTranslate();
        mainStartRot_ = mainCam->GetRotate();

        subStartPos_ = subCam->GetTranslate();

        Vector3 targetPos = followTarget_->GetWorldPosition();

        mainTargetPos_ = targetPos + subOffset_;
        subTargetPos_ = targetPos + subOffset_;

        transitionTimer_ = 0.0f;
    }

    transitionTimer_ += 1.0f / 60.0f;
    float t = transitionTimer_ / transitionDuration_;
    if (t > 1.0f) t = 1.0f;

    // イージング
    float easeT = t * t * (3.0f - 2.0f * t);

    // メインカメラ補間
    Vector3 mainPos = mainStartPos_ * (1 - easeT) + mainTargetPos_ * easeT;
    mainCam->SetTranslate(mainPos);

    // サブカメラも“少し遅れて”追従
    float followEase = 0.1f;
    Vector3 subNow = subCam->GetTranslate();
    Vector3 subPos = subNow + (subTargetPos_ - subNow) * followEase;
    subCam->SetTranslate(subPos);

    // 向き更新
    Vector3 dir = Normalize(mainTargetPos_ - mainPos);
    float yaw = atan2f(dir.x, dir.z);
    float pitch = -asinf(dir.y);
    mainCam->SetRotate({ pitch, yaw, 0.0f });

    // 完了処理
    if (t >= 1.0f) {
        cm->SetTypeview(ViewCameraType::Sub);
        cm->SetCameraMode(CameraMode::Default);
        isTransitioning_ = false;
    }
}


void GamePlayCamera::UpdateSubCameraFollow() {
    if (!followTarget_) return;

    Camera* subCam = CameraManager::GetInstance()->GetActiveCamera();
    if (!subCam) return;

    Vector3 targetPos = followTarget_->GetWorldPosition();
    Vector3 desiredPos = targetPos + subOffset_;

    // ※ 追従もイージング化
    float ease = 0.15f;
    Vector3 nowPos = subCam->GetTranslate();
    Vector3 newPos = nowPos + (desiredPos - nowPos) * ease;
    subCam->SetTranslate(newPos);

    Vector3 dir = targetPos - newPos;
    if (Length(dir) > 0.0001f) {
        dir = Normalize(dir);
        float yaw = atan2f(dir.x, dir.z);
        float pitch = -asinf(dir.y);
        subCam->SetRotate({ pitch, yaw, 0.0f });
    }
}
float GamePlayCamera::GetRailProgressRate() const {
    if (railInfo_.totalLength <= 0.0001f) return 0.0f;
    return std::clamp(currentRailLength_ / railInfo_.totalLength, 0.0f, 1.0f);
}