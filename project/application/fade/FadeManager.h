#pragma once
#include <memory>
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <CopylmageCommon.h>
#include <SpriteCommon.h>
#include <Sprite.h>

/// <summary>
/// フェードの種類
/// </summary>
enum class FadeType {
    None,    // 通常
	FadeIn,  // フェードイン
	FadeOut, // フェードアウト
};
/// <summary>
/// フェードの演出スタイル
/// </summary>
enum class FadeStyle {
    Normal,   // 全画面フェード
    SilhouetteExplode,    // 中心から外に飛び散って消えるフェード
    SilhouetteSlide,      // 左→右に消える、右→左に現れる（線状フェード）
};
/// <summary>
/// スプライトによるフェード演出
/// </summary>
struct FadeShape {
    std::unique_ptr<Sprite> sprite;
    Vector2 position;
    Vector2 size;
    float scale;
    float delay;   // このスプライトが拡大を始めるまでの遅延時間
    float rotation;        // 現在の回転角度（ラジアン）
    float rotationSpeed;   // 回転速度（ランダム）
};
/// <summary>
/// フェードを管理するマネージャークラス
/// </summary>
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
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize();
    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();
    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();  
    /// <summary>
    /// ImGuiデバッグ描画
    /// </summary> 
    void DrawImGui();
 private:
	/// <summary>
    /// 通常フェードの更新
    /// </summary>
    void UpdateNormalFade();     // 通常フェード
    /// <summary>
    /// スライドフェードの更新
    // </summary>
    void UpdateSilhouetteExplodeFade();
    /// <summary>
    /// 爆発フェードの更新
    /// </summary>
    void UpdateSilhouetteSlideFade();
 public:
   /// <summary>
   /// フェードイン開始
   /// </summary>
   void StartFadeIn(float duration, FadeStyle style);
   /// <summary>
   /// フェードアウト開始
   /// </summary>
   void StartFadeOut(float duration, FadeStyle style);
private:
    std::unique_ptr<Sprite> sprite_;  // フェード用スプライト
    FadeType fadeType_;               // 現在のフェード種別
    FadeStyle fadeStyle_ = FadeStyle::Normal; // 現在のフェードスタイル
    float timer_;                     // 経過時間
    float duration_;                  // フェード時間
    float alpha_;                     // 透明度
    bool isFading_;                   // フェード中フラグ
    bool fadeStart_;                  // 開始瞬間フラグ
    bool fadeEnd_;                    // 完了瞬間フラグ
    float t_;                         // 0〜1で進行度を表す補間値
    std::vector<FadeShape> shapes_;   // フェード演出スプライト群

public:
    // getter
    FadeType GetFadeType() const { return fadeType_; }
    bool IsFadeStart() const { return fadeStart_; }
    bool IsFadeEnd() const { return fadeEnd_; }    
    bool IsFading() const { return isFading_; }
};