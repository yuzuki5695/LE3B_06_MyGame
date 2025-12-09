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

    speed = 0.2f;        // 1フレームあたり移動距離
    movefige = true;
    currentSegment = 0;

    // 初期位置を最初の制御点に合わせる
    bezierPos_ = bezierPoints[0].controlPoint;
    prevForward = { 0, 0, 1 }; // 初期向き
    // メイン
    transform_ = { bezierPos_,LookAtRotation(prevForward) };
    // サブカメラ登録 
    std::vector<CameraTransform> subCams = { { {2, 0, -3}, {0, 0, 0} } };
    // サブカメラを追加
    AddSubCameras(subCams);

    // メインカメラは追従モードにする
    CameraManager::GetInstance()->SetMode(CameraMode::Follow);

//    CameraManager::GetInstance()->SetTypeview(ViewCameraType::Sub);
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
    if (CameraManager::GetInstance()->GetTypeview() ==ViewCameraType::Main && CameraManager::GetInstance()->GetMode() == CameraMode::Follow) {
        if (CheckAndResumeMovement()) 
        UpdateBezierMovement();
        transform_.translate = bezierPos_;
        UpdateCameraRotation();
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
