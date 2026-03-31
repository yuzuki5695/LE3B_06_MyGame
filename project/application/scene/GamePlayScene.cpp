#include "GamePlayScene.h"
#include <SceneManager.h>
#include <TextureManager.h>
#include <ModelManager.h>
#include <SpriteCommon.h>
#include <Object3dCommon.h>
#include <CameraManager.h>

void GamePlayScene::Finalize() {
 //   BulletManager::GetInstance()->Finalize();  // 弾の解放処理
 //   FadeManager::GetInstance()->Finalize();    //  フェードマネージャの解放処理
 //   EventManager::GetInstance()->Finalize();   //  イベントマネージャの解放処理
 //   StageManager::GetInstance()->Finalize();   // ステージマネージャの解放処理
	//UIManager::GetInstance()->Finalize();      // UIマネージャの解放処理
}

void GamePlayScene::Initialize() {
    // カメラマネージャの初期化
    CameraManager::GetInstance()->Initialize(CameraTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }));
    CameraManager::GetInstance()->SetTypeview(ViewCameraType::Main);

}

void GamePlayScene::Update() {       
    // ---------------------------

    CameraManager::GetInstance()->Update();
#pragma region 全てのObject3d個々の更新処理
    //// 終了しない限り更新処理
    //particles_->Update();
#pragma endregion 全てのObject3d個々の更新処理

#pragma region 全てのSprite個々の更新処理

#pragma endregion 全てのSprite個々の更新処理

#pragma region  ImGuiの更新処理開始
#ifdef USE_IMGUI 
//    ImGui::Begin("=== GamePlayScene Debug ===");

#endif // USE_IMGUI
#pragma endregion ImGuiの更新処理終了 
}
///====================================================
/// 描画処理
///====================================================
void GamePlayScene::Draw() {
#pragma region 全てのObject3d個々の描画処理
    // 箱オブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
  //  SkyboxCommon::GetInstance()->Commondrawing();
 //   Box_->Draw();
    // 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
    Object3dCommon::GetInstance()->Commondrawing();
   // StageManager::GetInstance()->Draw();

    //if (enemy_) {
    //    enemy_->Draw();
    //}

    //wall->Draw();
    //// イベントマネージャの描画処理
    //EventManager::GetInstance()->Drawo3Dbject();

    // パーティクルの描画準備。パーティクルの描画に共通のグラフィックスコマンドを積む 
 ///   ParticleCommon::GetInstance()->Commondrawing();
    //ParticleManager::GetInstance()->Draw();
#pragma endregion 全てのObject3d個々の描画処理

#pragma region 全てのSprite個々の描画処理
    // Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
    SpriteCommon::GetInstance()->Commondrawing();
    //// プレイヤーのUIスプライト描画
    //if (!end) {

    //// イベントマネージャの描画処理
    //EventManager::GetInstance()->Draw2DSprite();
    //// フェードマネージャの描画
    //FadeManager::GetInstance()->Draw();
#pragma endregion 全てのSprite個々の描画処理
}

