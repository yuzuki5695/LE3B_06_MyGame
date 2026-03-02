#pragma once
#include<SrvManager.h>
#ifdef USE_IMGUI
#include<externals/imgui/imgui_impl_dx12.h>
#include<externals/imgui/imgui_impl_win32.h>
#endif // USE_IMGUI

// 前方宣言
class WinApp;
class DirectXCommon;

///<summary>
/// ImGUIの管理
/// ImGuiの初期化、受付、描画を管理するマネージャクラス（シングルトン）
///</summary>
class ImGuiManager
{
private:
	static ImGuiManager* instance;

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(ImGuiManager&) = delete;
	ImGuiManager& operator=(ImGuiManager&) = delete;
public: // メンバ関数
	// シングルトンインスタンスの取得
	static ImGuiManager* GetInstance();
	///<summary>
	/// 初期化
	///</summary>
	// 初期化
	void Initialize(WinApp* winApp, DirectXCommon* DxCommon, SrvManager* srvManager);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 画面への描画
	/// </summary>
	void Draw();
	/// <summary>
	/// 日本語フォントのセットアップ
	/// </summary>
	void SetupFonts();
	// 日本語フォント関連の定数
	static const std::string kFontPath;  // フォントファイルのパス
	static const float kDefaultFontSize; // デフォルトのフォントサイズ	

private: // メンバ変数
	// ポインタ
	WinApp* winApp_;
	DirectXCommon* DxCommon_;
	SrvManager* srvManager_;
	// imgui番号
	uint32_t imguiindex;
	// ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE ImGuiHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE ImGuiHandleGPU;
};