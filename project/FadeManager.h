#pragma once
#include <memory>
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <CopylmageCommon.h>
#include <SpriteCommon.h>
#include <Sprite.h>

enum class FadeType {
    None,    // 通常
	FadeIn,  // フェードイン
	FadeOut, // フェードアウト
};

// フェードの演出スタイル（新規）
enum class FadeStyle {
    Normal,   // 全画面フェード
    Circle,   // 円フェード
};

struct FadeShape {
    std::unique_ptr<Sprite> sprite;
    Vector2 position;
    Vector2 size;
    float scale;
    float delay;   // このスプライトが拡大を始めるまでの遅延時間
};

// フェードマネージャー
class FadeManager {
private:
	static std::unique_ptr<FadeManager> instance;

	FadeManager(FadeManager&) = delete;
	FadeManager& operator=(FadeManager&) = delete;
public: // メンバ関数
	FadeManager() = default;
	~FadeManager() = default;
	// シングルトンインスタンスの取得
	static FadeManager* GetInstance();
	// 終了
	void Finalize();
    // 初期化
    void Initialize();
    // 毎フレーム更新
    void Update();
    // 描画
    void Draw();
    
    void DrawImGui(); // 👈 追加：ImGui描画
        
	void UpdateNormalFade();     // 通常フェード
    void UpdateSilhouetteFade(); // シルエットフェード
    
    void StartFadeIn(float duration, FadeStyle style);
    void StartFadeOut(float duration, FadeStyle style);


private:
    std::unique_ptr<Sprite> sprite_;
    FadeType fadeType_;
    FadeStyle fadeStyle_ = FadeStyle::Normal;
    float timer_;
    float duration_;
    float alpha_;
    bool isFading_;
    bool fadeStart_;  // フェード開始瞬間
    bool fadeEnd_;    // フェード完了瞬間
    float t_;

    std::vector<FadeShape> shapes_;
public:
    // getter
    FadeType GetFadeType() const { return fadeType_; }

    bool IsFadeStart() const { return fadeStart_; }
    bool IsFadeEnd() const { return fadeEnd_; }    
};