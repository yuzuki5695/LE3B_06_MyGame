#pragma once
#include "BaseBullet.h"
#include <vector>
#include <memory>

class BulletManager {
private:
    static BulletManager* instance;
    BulletManager(BulletManager&) = delete;
    BulletManager& operator=(BulletManager&) = delete;
public: // メンバ関数 
    BulletManager() = default;
    ~BulletManager() = default;
    // シングルトンインスタンスの取得
    static BulletManager* GetInstance();
    // 初期化・終了
    void Initialize();
    void Finalize();

    // 更新・描画
    void Update();
    void Draw();

    // 弾を追加（プレイヤーから発射など）
    void AddBullet(std::unique_ptr<BaseBullet> bullet);
    // 弾リスト取得（衝突処理などに使う場合）
    const std::vector<std::unique_ptr<BaseBullet>>& GetBullets() const;

private:
    std::vector<std::unique_ptr<BaseBullet>> bullets_;
};