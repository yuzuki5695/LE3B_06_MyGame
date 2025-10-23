#include "CameraManager.h"
#include <Object3dCommon.h>
#include <ParticleCommon.h>
#include <MatrixVector.h>
#ifdef USE_IMGUI
#include <ImGuiManager.h>
#endif // USE_IMGUI

using namespace MatrixVector;

// 静的メンバ変数の定義
std::unique_ptr<CameraManager> CameraManager::instance = nullptr;

// シングルトンインスタンスの取得
CameraManager* CameraManager::GetInstance() {
    if (!instance) {
        instance = std::make_unique<CameraManager>();
    }
    return instance.get();
}

// 終了
void CameraManager::Finalize() {
    instance.reset();  // `delete` 不要
}
// 初期化
void CameraManager::Initialize(CameraTransform transform) {
    // 初期状態はデフォルトカメラ
    currentMode_ = CameraMode::Default;
	useFollowCamera_ = false;
    
    // すべてのカメラを作っておく
    defaultCamera_ = new Camera();
    defaultCamera_->SetTranslate(transform.translate);
    defaultCamera_->SetRotate(transform.rotate);
    // オブジェクト追従カメラの生成
    followCamera_ = new Camera();
    // ゲームカメラの生成
	gameCamera_ = new GameCamera();
	gameCamera_->Initialize();
    moveFlag = false; 
    gameCamera_->Setmovefige(moveFlag);
	// イベントカメラの生成   
    eventCamera_ = new Camera();
}
// 更新処理
void CameraManager::Update() {
    switch (currentMode_) {
    case CameraMode::GamePlay: 
        if (gameCamera_) {
			gameCamera_->Update();
        }
        break;
    case CameraMode::Follow:
        if (followCamera_) {
            if (target_) {
                Vector3 targetPos = target_->GetTranslate();
                Vector3 offset(0.0f, 3.0f, -30.0f);
                Vector3 cameraPos = targetPos + offset;

                followCamera_->SetTranslate(cameraPos);

                Vector3 toTarget = targetPos - cameraPos;
                float angleY = std::atan2(toTarget.x, toTarget.z);
                followCamera_->SetRotate(Vector3(0.0f, angleY, 0.0f));
            }
            followCamera_->Update();
        }
        break;
    case CameraMode::Event:
        if (eventCamera_ && target_) {
            // カメラの位置を固定（例：原点）
            Vector3 cameraPos = { 3.0f, -5.0f, -100.0f };
            eventCamera_->SetTranslate(cameraPos);

            // プレイヤー（ターゲット）の位置
            Vector3 targetPos = target_->GetTranslate();

            // カメラからターゲットへの方向ベクトル
            Vector3 toTarget = targetPos - cameraPos;

            // 正規化
            Normalize(toTarget);

            // 水平角度（Y軸）と垂直角度（X軸）を算出
            float angleY = std::atan2(toTarget.x, toTarget.z);
            float distanceXZ = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
            float angleX = -std::atan2(toTarget.y, distanceXZ);

            // 回転設定
            eventCamera_->SetRotate(Vector3(angleX, angleY, 0.0f));
            eventCamera_->Update();

            if (gameCamera_->GetbezierPos().z >= -100.0f) {
				currentMode_ = CameraMode::GamePlay;
            }
        }
        break;
    case CameraMode::Default:
        if (defaultCamera_) {
            // デフォルトカメラの位置と回転を更新
            defaultCamera_->Update();
        }
        break;
    }
}
// 追従対象をセット（nullptrなら追従なし）
void CameraManager::SetTarget(Object3d* target) {
    target_ = target;
}
// カメラモード切替
void CameraManager::ToggleCameraMode(bool followMode) {
    useFollowCamera_ = followMode;
}
// ImGui描画
void CameraManager::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("CameraManager");

    // ラジオボタンでモード選択
    int mode = static_cast<int>(currentMode_);
    ImGui::Text("Camera Mode");
    if (ImGui::RadioButton("Default", mode == 0)) {
        mode = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Follow", mode == 1)) {
        mode = 1;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("GamePlay", mode == 2)) {
        mode = 2;
    }
    ImGui::SameLine();  
    if (ImGui::RadioButton("Event", mode == 3)) { mode = 3; } 

    // モードが変わったら切り替える
    CameraMode newMode = static_cast<CameraMode>(mode);
    if (newMode != currentMode_) {
        currentMode_ = newMode;
        SetActiveCamera();
    }

    // 現在のカメラを取得して編集
    Camera* activeCamera = nullptr;
    const char* modeName = "";
    const char* label = "Translate";
    const char* label2 = "Rotate";

    switch (currentMode_) {
    case CameraMode::Default:
        activeCamera = defaultCamera_;
        modeName = "Default";
        break;
    case CameraMode::Follow:
        activeCamera = followCamera_;
        modeName = "Follow";
        break;
    case CameraMode::GamePlay:
        activeCamera = gameCamera_->Getcamera();
        modeName = "GamePlay";
        break;
    case CameraMode::Event:   // ★ イベントカメラの追加
        activeCamera = eventCamera_;
        modeName = "Event";
        break;
    }
    ImGui::Text("Current Mode: %s", modeName);

    if (activeCamera) {
        Vector3 pos = activeCamera->GetTranslate();
        Vector3 rotate = activeCamera->GetRotate();
        if (ImGui::DragFloat3(label, &pos.x, 0.01f)) {
            activeCamera->SetTranslate(pos);
        }
        if (ImGui::DragFloat3(label2, &rotate.x, 0.01f)) {
            activeCamera->SetRotate(rotate);
        }
    }

    if (ImGui::Checkbox("isBezier", &moveFlag)) {
        gameCamera_->Setmovefige(moveFlag);
    }
    Vector3 bezierPos = gameCamera_->GetbezierPos();
    Vector3 rotate = gameCamera_->Getcamera()->GetRotate();
    if (ImGui::DragFloat3("BezierTranslate", &bezierPos.x, 0.01f)) {
        gameCamera_->SetbezierPos(bezierPos);
    }
    ImGui::End();
#endif // USE_IMGUI
}
// アクティブカメラを取得
Camera* CameraManager::GetActiveCamera() {
    switch (currentMode_) {
    case CameraMode::Follow:
        return followCamera_ ? followCamera_ : defaultCamera_;
    case CameraMode::GamePlay:
        return gameCamera_ ? gameCamera_->Getcamera() : nullptr;
      case CameraMode::Event:
        return eventCamera_ ? eventCamera_ : defaultCamera_;
    case CameraMode::Default:
    default:
        return defaultCamera_;
    }
}
// カメラモード設定
void CameraManager::SetCameraMode(CameraMode mode) {
    currentMode_ = mode;
    SetActiveCamera(); // カメラ共通リソースへ反映
}
// アクティブカメラを共通リソースに設定
void CameraManager::SetActiveCamera() {
    Camera* active = GetActiveCamera();
    if (active) {
        Object3dCommon::GetInstance()->SetDefaultCamera(active);
        ParticleCommon::GetInstance()->SetDefaultCamera(active);
    }
}