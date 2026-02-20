#include "Framework.h"
#include<D3DResourceLeakChecker.h>
#include<DirectXCommon.h>
#include<ModelCommon.h>
#include<SrvManager.h>
#include<SceneFactory.h>
#include<ShaderCompiler.h>
#include<CopylmageCommon.h>
#include<Controller.h>
#include<psapi.h>
#include<SkyboxCommon.h>
#include <FileSearcher.h>
#include <ManifestExporter.h>

void Framework::Run() {
    // ゲームの初期化
    Initialize();

    while (true) // ゲームループ
    {
        // 毎フレーム更新
        Update();
        // 終了リクエストが来たらループを抜ける
        if (IsEndRequst()) {
            break;
        }
        // 描画
        Draw();
    }
    // ゲームの終了
    Finalize();
}

void Framework::Finalize() {
    // シーンファクトリの解放 
    sceneFactory_.reset();
    // シーンマネージャの解放
    SceneManager::GetInstance()->Finalize();
    // 基盤システムの解放
    SkyboxCommon::GetInstance()->Finalize();
    ParticleCommon::GetInstance()->Finalize();
    SpriteCommon::GetInstance()->Finalize();
    Object3dCommon::GetInstance()->Finalize();
    // 入力解放
    Input::GetInstance()->Finalize();
    // レンダーテクスチャ共通部の解放
    CopylmageCommon::GetInstance()->Finalize();
    // パーティクルマネージャの終了
    ParticleManager::GetInstance()->Finalize();
    // テクスチャマネージャの終了
    TextureManager::GetInstance()->Finalize();
    // 3Dモデルマネージャの終了
    ModelManager::GetInstance()->Finalize();
    // ImGuiマネージャの解放
    ImGuiManager::GetInstance()->Finalize();   
    // DSVマネージャの開放
    dsvManager.reset();
    // RTVマネージャの開放
    rtvManager.reset();
    // SRVマネージャの開放
    srvManager.reset();
    // DirectXの解放
    dxCommon.reset();
    // WindowsAPIの解放
    winApp.reset();
    //リソースリークチェック
    D3DResourceLeakChecker leakCheck;
}

void Framework::Initialize() {
    OutputDebugStringA("Hello,Directx!\n");
    // ウィンドウ作成
    // WindowsAPIの初期化
    winApp = std::make_unique <WinApp>();    
    // ウィンドウのタイトルを変更
	winApp->SetWindowTitle(L"LE3B_07_オノデラ_ユヅキ_Skyraider");
    winApp->Initialize();
    // DirectXの初期化
    dxCommon = std::make_unique <DirectXCommon>();
    dxCommon->Initialize(winApp.get()); 
    // シェーダーコンパイルの初期化
    ShaderCompiler::GetInstance()->Initialize();
    // 音声読み込み
    SoundLoader::GetInstance()->Initialize();
    // 音声再生
    SoundPlayer::GetInstance()->Initialize(SoundLoader::GetInstance());
    // SRVマネージャーの初期化
    srvManager = std::make_unique <SrvManager>();
    srvManager->Initialize(dxCommon.get());   
    // RTVマネージャーの初期化
    rtvManager = std::make_unique <RtvManager>();
    rtvManager->Initialize(dxCommon.get());      
    // DSVマネージャーの初期化
    dsvManager = std::make_unique <DsvManager>();
    dsvManager->Initialize(dxCommon.get());
    // ImGuiマネージャの初期化
    ImGuiManager::GetInstance()->Initialize(winApp.get(), dxCommon.get(), srvManager.get());
    // テクスチャマネージャの初期化
    TextureManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get());
    // 3Dモデルマネージャの初期化
    ModelManager::GetInstance()->Initialize(dxCommon.get());
    // パーティクルマネージャの初期化
    ParticleManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get()); 


    // 1. 検索クラス
    FileSearcher searcher("Resources");
    
    // 2. 出力クラス
    ManifestExporter exporter;

    // 3. 実行：検索した結果をそのまま出力に渡す
    auto files = searcher.GetAllFiles({ "EditorTemp", ".git" });
    exporter.Export("Resources/manifest.json", files);

#pragma region 基盤システムの初期化

    // 入力の初期化
    Input::GetInstance()->Initialize(winApp.get());

    // スプライト共通部の初期化
    SpriteCommon::GetInstance()->Initialize(dxCommon.get(),dsvManager.get());

    // 3Dオブジェクト共通部の初期化
    Object3dCommon::GetInstance()->Initialize(dxCommon.get(),dsvManager.get());

    // パーティクル共通部の初期化
    ParticleCommon::GetInstance()->Initialize(dxCommon.get(),dsvManager.get());
 
	// レンダーテクスチャ共通部の初期化
    CopylmageCommon::GetInstance()->Initialize(dxCommon.get(), srvManager.get(),rtvManager.get(),dsvManager.get());
    
    // 箱の共通部の初期化
    SkyboxCommon::GetInstance()->Initialize(dxCommon.get(),dsvManager.get());

#pragma endregion 基盤システムの初期化
}

void Framework::Update() {
    // Windowのメッセージ処理
    if (winApp->ProcessMessage()) {
        // ゲームループを抜ける
        endRequst_ = true;
    }
    // 入力の更新
    Input::GetInstance()->Update();
    // コントローラの更新
    Controller::GetInstance()->Update();
    // ImGuiの受付開始
    ImGuiManager::GetInstance()->Begin();
    // シーンマネージャの更新処理
    SceneManager::GetInstance()->Update(); 
    DrawDebug();
    // ImGuiの描画前準備
    ImGuiManager::GetInstance()->End();
}

void Framework::Draw() {
    //  描画用のDescriptorHeapの設定
    srvManager->PreDraw();
    // レンダーテクスチャをレンダーターゲットにして描画開始準備
    rtvManager->PreDrawRenderTexture();
    dxCommon->PreDrawRenderTexture(rtvManager->GetRtvHandle(2),dsvManager->GetDsvDescriptorHeap(), rtvManager->GetkRenderTargetClearValue());
    // シーンマネージャの描画処理
    SceneManager::GetInstance()->Draw();
    // レンダーテクスチャをSRVとして使うための状態に遷移
    rtvManager->PostDrawRenderTexture();
    //  DirectXの描画準備。全ての描画に共通のグラフィックスコマンドを積む
    // ここから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = dxCommon->GetSwapChain()->GetSwapChain()->GetCurrentBackBufferIndex();
    dxCommon->PreDraw(rtvManager->GetRtvHandle(backBufferIndex),dsvManager->GetDsvDescriptorHeap());
    // ポストエフェクト描画（レンダーテクスチャ → 画面） 
    CopylmageCommon::GetInstance()->Commondrawing(srvManager.get());
}

void Framework::DrawDebug() {
#ifdef USE_IMGUI
    // --- A. 基本設定 ---
    const float kGameViewW = 640.0f;
    const float kGameViewH = 360.0f;

    // --- B. 全画面 DockSpace (エディタの土台) ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID); 

    // ルートウィンドウの設定 (メニューバーあり、移動・リサイズ不可、背景あり)
    ImGuiWindowFlags root_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
                                  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("EditorRoot", nullptr, root_flags);
    ImGui::PopStyleVar(3);

    // DockSpaceの設置
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    
    // 【重要】Passthruを外すことで、後ろに透けていた本来のゲーム画面を隠し、
    // グレーの背景（またはImGuiウィンドウ）で塗りつぶします。
    ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_None; 
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dock_flags);

    // メニューバー
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Alt+F4")) { endRequst_ = true; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End(); // EditorRoot終了

    // --- C. 固定ゲームビュー (左上に配置) ---
    // メニューバーの高さを考慮して配置 (必要に応じて y + 20px 調整)
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(kGameViewW, kGameViewH), ImGuiCond_Always);
    
    // NoDockingを入れて、勝手に中央に吸い込まれないようにする
    ImGuiWindowFlags game_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
                                  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | 
                                  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                                  ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    // ゲーム画面を際立たせるために背景を黒にする
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1)); 

    ImGui::Begin("GameView_Fixed", nullptr, game_flags);
    {
        uint32_t srvIdx = CopylmageCommon::GetInstance()->GetSrvIndex();
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = srvManager->GetGPUDescriptorHandle(srvIdx);

        // 型変換の安全な形 (uintptr_t 経由)
        ImTextureID textureID = (ImTextureID)(void*)(uintptr_t)srvHandle.ptr;

        // レンダーテクスチャを描画
        ImGui::Image(textureID, ImVec2(kGameViewW, kGameViewH));
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // --- D. 各種パネル ---
    // これらは DockSpace 内で自由に移動・ドッキング可能です
    ImGui::Begin("Inspector");
    ImGui::Text("Transform");
    // ここに SceneManager 等から持ってきた数値を表示
    ImGui::End();

    ImGui::Begin("Asset");
    ImGui::Text("File list...");
    ImGui::End();

#endif
}