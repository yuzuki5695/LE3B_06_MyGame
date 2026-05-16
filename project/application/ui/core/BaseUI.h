#pragma once
#include <memory>
#include <vector>
#include <Sprite.h>

namespace MyGame {
    /// <summary>
    /// UIの基底クラス
    /// 初期化・更新・描画のインターフェースを定義
    /// </summary>
    class BaseUI {
	public: // メンバ関数
        /// <summary>
		/// デストラクタ
        /// </summary>
        virtual ~BaseUI() = default;
        /// <summary>
		/// 初期化処理
        /// </summary>
        virtual void Initialize() = 0;
        /// <summary>
		/// 更新処理
        /// </summary>
        virtual void Update() = 0;
        /// <summary>
		/// 描画処理
        /// </summary>
        virtual void Draw() = 0;
		// UIの表示・更新を行うかどうか設定
        void SetActive(bool isActive) { isActive_ = isActive; }
		// UIの表示・更新を行うかどうか取得
        bool IsActive() const { return isActive_; }
        // どのシーン用のUIか設定・取得
        void SetTargetSceneName(const std::string& name) { sceneName_ = name; }
		// どのシーン用のUIか取得
        const std::string& GetTargetSceneName() const { return sceneName_; }
	protected: // メンバ変数
        bool isActive_ = true;   // 表示・更新を行うかどうか
        std::string sceneName_; //  シーン名
    };
}