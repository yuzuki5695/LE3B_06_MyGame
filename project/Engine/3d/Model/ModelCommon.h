#pragma once
#include <DirectXCommon.h>

namespace MyEngine {
	// 3Dモデル共通部
	class ModelCommon {
	public: // メンバ関数
		/// <summary>
		/// モデル共通部の初期化
		/// </summary>
		/// <param name="dxCommon">DirectXの基本機能（Device/CommandList等）を管理するクラスのポインタ</param>
		/// <remarks>
		/// 内部でグラフィックスパイプラインの生成や、デフォルトテクスチャの読み込み等を行う
		/// </remarks>
		void Initialize(DirectXCommon* dxCommon);
	private: // メンバ変数
		// ポインタ
		DirectXCommon* dxCommon_;
	public: // アクセッサ
		// getter
		DirectXCommon* GetDxCommon() { return  dxCommon_; }
	};
}