#include "ImGuiManager.h"
#include<WinApp.h>
#include<DirectXCommon.h>
#include <filesystem>
#include <iostream>

using namespace Microsoft::WRL;

ImGuiManager* ImGuiManager::instance = nullptr;

ImGuiManager* ImGuiManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ImGuiManager;
	}
	return instance;
}

void ImGuiManager::Finalize() {
#ifdef USE_IMGUI
	// ImGuiの終了処理。後始末
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // USE_IMGUI
	delete instance;
	instance = nullptr;
}

void ImGuiManager::Initialize([[maybe_unused]] WinApp* winApp, [[maybe_unused]] DirectXCommon* DxCommon, [[maybe_unused]] SrvManager* srvManager) {
#ifdef USE_IMGUI
	// 引数で受け取ってメンバ変数に記録する
	this->winApp_ = winApp;
	this->DxCommon_ = DxCommon;
	this->srvManager_ = srvManager;

	// ImGuiのコンテキストを生成
	ImGui::CreateContext();

	// iniファイルを作らないように設定
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	// インデックスを確保
	imguiindex = srvManager_->Allocate();
	ImGuiHandleCPU = srvManager_->GetCPUDescriptorHandle(imguiindex);
	ImGuiHandleGPU = srvManager_->GetGPUDescriptorHandle(imguiindex);

	// Win32用初期化
	ImGui_ImplWin32_Init(winApp_->Gethwnd());
	// DirectX12用初期化
	ImGui_ImplDX12_Init(
		DxCommon_->GetDevice().Get(),
		static_cast<int>(DxCommon_->GetSwapChain()->GetResources().size()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvManager_->GetDescriptorHeap().Get(),
		ImGuiHandleCPU,
		ImGuiHandleGPU
	);

	///=========================================
	/// フォントの読み込み処理
	///=========================================

	// 日本語を扱えれるようFontフォルダからフォントを読み込むためのファイルパス
	std::string fontPath = "Resources/Font/meiryo.ttc";  // 使用するフォント(今回使用しているのはWindows専用のメイリオ(Meiryo)フォント。Vista(2007)以降Windowsに標準搭載)

	//	フォント読み込み時のオプション
	ImFontConfig fontConfig;
	fontConfig.OversampleH = 2;   // フォントを高品質にレンダリングするための設定
	fontConfig.OversampleV = 2;   // ぼやけを軽減し、文字がより綺麗になる
	fontConfig.PixelSnapH = true; // ピクセル境界に文字をスナップさせて、にじみを減らす

	// フォント読み込み
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		fontPath.c_str(),                   // フォントを追加
		15.0f,                              // フォントサイズ
		&fontConfig,                        // オプション
		io.Fonts->GetGlyphRangesJapanese()  // 日本語の文字範囲を読み込む
	);
	///=========================================
	/// 読み込み失敗チェック
	///=========================================
	assert(font && "Failed to load font!");

	// フォントテクスチャをDX12にアップロード
	ImGui_ImplDX12_CreateDeviceObjects();
#endif // USE_IMGUI
}

void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // USE_IMGUI
}

void ImGuiManager::End() {
#ifdef USE_IMGUI
	// 描画前準備
	ImGui::Render();
#endif // USE_IMGUI
}

void ImGuiManager::Draw() {
#ifdef USE_IMGUI
	ComPtr<ID3D12GraphicsCommandList> commandList = DxCommon_->GetCommandList();
	// デスクリプタヒープの配列をセットするコマンド
	ComPtr<ID3D12DescriptorHeap> ppheaps[] = { srvManager_->GetDescriptorHeap().Get() };
	commandList->SetDescriptorHeaps(_countof(ppheaps), ppheaps->GetAddressOf());
	// 描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
#endif // USE_IMGUI
}