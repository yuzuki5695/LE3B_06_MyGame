#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION   0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#include <WinApp.h>
#include <memory>

namespace MyEngine {
	/// <summary>
	/// キーボードとマウスの入力を管理するクラス。
	/// </summary>
	class Input	{
	private: // シングルトンインスタンス
		static std::unique_ptr<Input> instance; // インスタンス

		Input(Input&) = delete;
		Input& operator=(Input&) = delete;
	public: // メンバ関数
		Input() = default;
		~Input() = default;

		// シングルトンインスタンスの取得
		static Input* GetInstance();
		/// <summary>		
		/// 終了
		/// </summary>
		void Finalize();
		// namespace省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="winApp"></param>
		void Initialize(WinApp* winApp);			
		/// <summary>
		/// キーの状態を更新する。
		/// </summary>
		void Update();
		/// <summary>
		/// キーの押下をチェック
		/// </summary>
		/// <param name="keyNumber"キー番号( DIK_0 等)</param>
		bool PushKey(BYTE keyNumber);
		/// <summary>
		/// キーのトリガーをチェック
		/// </summary>
		/// <param name="keyNumber"キー番号( DIK_0 等)</param>
		/// <returns>トリガーか</returns>
		bool TriggerKey(BYTE keyNumber);
		/// <summary>
		/// マウスの座標を取得
		/// </summary>
		POINT GetMousePosition() const { return mousePosition_; }	
		/// <summary>
		/// ImGuiデバッグ描画
		/// </summary>
		void DrawImGui();
	private: // メンバ変数	
		// ポインタ
		WinApp* winApp_ = nullptr;
		//キーボードのデバイス
		ComPtr<IDirectInputDevice8> keyboard;
		// 全キーの状態
		BYTE key[256] = {};
		// 前回の全キーの状態
		BYTE keyPre[256] = {};
		// DirectInputのインスタンス生成
		ComPtr<IDirectInput8> dinput = nullptr;
		// マウスのデバイス
		ComPtr<IDirectInputDevice8> mouse;
		// マウスの状態（DirectInput 専用構造体）
		DIMOUSESTATE mouseState = {};
		// 画面上のマウス座標（独自で保持）
		POINT mousePosition_ = { 0, 0 };
		//　マウスを使うかどうか
		bool useMouseInput_ = false;
	public: // getter setter
		bool GetMouseInput() { return useMouseInput_; }
		void SetMouseInput(bool mouse) { useMouseInput_ = mouse; }
		const DIMOUSESTATE& GetMouseState() const { return mouseState; }
	};
}